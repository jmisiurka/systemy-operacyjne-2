#include "../include/OrderDispatcher.h"
#include "../include/Factory.h"
#include "Gear.h"
#include "Logger.h"
#include "Order.h"
#include <mutex>
#include <ncurses.h>
#include <string>
#include <thread>

const std::string OrderDispatcher::id = "Dispatcher";

OrderDispatcher::OrderDispatcher(std::vector<Furnace *> &furnaces,
                                 std::vector<MillingMachine *> &machines,
                                 Factory *factory)
    : furnaces(furnaces), machines(machines), factory(factory) {}

OrderDispatcher::~OrderDispatcher() {
  std::unique_lock<std::mutex> lock(orders_mutex);
  for (auto order : orders) {
    order->finalize(); // to delete the gears
    delete order;
  }
}

void OrderDispatcher::start() {
  this->working = true;
  std::thread incomingOrdersThread(&OrderDispatcher::incomingOrdersLoop, this);
  std::thread outgoingOrdersThread(&OrderDispatcher::outgoingOrdersLoop, this);
  std::thread handleFinishedGearsThread(&OrderDispatcher::handleFinishedGears,
                                        this);

  incomingOrdersThread.detach();
  outgoingOrdersThread.detach();
  handleFinishedGearsThread.detach();
}

void OrderDispatcher::stop() { this->working = false; }

void OrderDispatcher::incomingOrdersLoop() {
  while (working) {

    Order *currentOrder = nullptr;

    std::unique_lock<std::mutex> orders_lock(orders_mutex);
    if (!orders.empty()) {
      // find an order to start working on
      for (int i = 0; i < orders.size(); i++) {
        if (orders[i]->gearsInQueue()) {
          currentOrder = orders[i];
          break;
        }
      }
      orders_lock.unlock();

      if (!this->working) {
        // orders_lock.unlock();
        return;
      }

      if (currentOrder == nullptr) {
        continue;
      }

      Logger::log(this->id, "starting to work on order with ID " +
                                std::to_string(currentOrder->getID()) + ".");

      while (currentOrder->gearsInQueue()) {
        std::unique_lock<std::mutex> lock(factory->furnaces_mutex);
        // wait if there are no non-empty furnaces
        factory->empty_furnaces.wait(lock, [this] {
          for (auto furnace : furnaces) {
            if (furnace->empty()) {
              return true;
            }
          }
          return false;
        });

        if (!this->working) {
          lock.unlock();
          return;
        }

        // dummy mutex and lock for waiting
        std::mutex m;
        std::unique_lock<std::mutex> l(m);
        // if power is off, wait until it turns on
        factory->power_on.wait(l,
                               [this] { return factory->getPowerAvailable(); });

        for (auto furnace : furnaces) {
          if (furnace->empty()) {
            // take the gear from order and start production process

            furnace->startCasting(currentOrder->popGear());

            // if the current order is empty after starting production process,
            // stop
            if (!currentOrder->gearsInQueue()) {
              lock.unlock();
              break;
            }
          }
        }

        if (lock.owns_lock()) {
          lock.unlock();
        }
      }
    }

    // orders_lock.unlock();
  }
}

void OrderDispatcher::outgoingOrdersLoop() {
  while (working) {
    std::unique_lock<std::mutex> lock(orders_mutex);

    if (!this->working) {
      lock.unlock();
      return;
    }

    // dummy mutex and lock for waiting
    std::mutex m;
    std::unique_lock<std::mutex> l(m);
    // if power is off, wait until it turns on
    factory->power_on.wait(l, [this] { return factory->getPowerAvailable(); });

    if (!orders.empty()) {
      for (int i = 0; i < orders.size(); i++) {
        if (this->orders[i]->readyToSend()) {
          this->orders[i]->finalize();
          delete this->orders[i];
          this->orders.erase(orders.begin() + i);
          finished_orders += 1;
          break;
        }
      }
    }

    lock.unlock();
  }
}

void OrderDispatcher::handleFinishedGears() {
  while (working) {
    std::unique_lock<std::mutex> lock(factory->machines_mutex);

    // wait until there is a machine that finished processing
    factory->finished_machines.wait(lock, [this] {
      for (auto machine : machines) {
        if (machine->finished()) {
          return true;
        }
      }

      return false;
    });

    if (!this->working) {
      lock.unlock();
      return;
    }

    // dummy mutex and lock for waiting
    std::mutex m;
    std::unique_lock<std::mutex> l(m);
    // if power is off, wait until it turns on
    factory->power_on.wait(l, [this] { return factory->getPowerAvailable(); });

    for (auto machine : machines) {
      if (machine->finished()) {
        Gear *gear = machine->removeGear();
        gear->setState(GearState::PACKED);
        Logger::log(this->id, "packed gear from " + machine->getId());

        factory->empty_machines.notify_one();

        lock.unlock();
        break;
      }
    }

    if (lock.owns_lock()) {
      lock.unlock();
    }
  }
}

void OrderDispatcher::addOrder(Order *order) {
  std::unique_lock<std::mutex> lock(orders_mutex);
  this->orders.push_back(order);
  lock.unlock();

  Logger::log(this->id, "new order with ID " + std::to_string(order->getID()) +
                            " added.");
}

// std::mutex &OrderDispatcher::getOrderMutex() { return this->orders_mutex; }

int OrderDispatcher::getQueueLength() const {
  // std::unique_lock<std::mutex> lock(orders_mutex);
  return this->orders.size();
}

int OrderDispatcher::getFinishedOrders() const { return finished_orders; }
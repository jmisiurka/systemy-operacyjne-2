#include "Order.h"
#include "Gear.h"
#include "Logger.h"
#include <mutex>
#include <queue>
#include <string>
#include <vector>

int Order::count = 0;

Order::Order() {
  this->ID = Order::count;
  Order::count++;
  this->ordered_gears = std::queue<Gear *>();
  this->started_gears = std::vector<Gear *>();
}

Order::Order(std::queue<Gear *> ordered_gears) {
  this->ID = Order::count;
  Order::count++;
  this->ordered_gears = ordered_gears;
  this->started_gears = std::vector<Gear *>(ordered_gears.size());
}

Order::Order(const Order &order) {
  this->ID = order.ID;
  this->ordered_gears = order.ordered_gears;
  this->started_gears = order.started_gears;
}

Order &Order::operator=(const Order &&order) {
  this->ID = std::move(order.ID);
  this->ordered_gears = std::move(order.ordered_gears);
  this->started_gears = std::move(order.started_gears);
  return *this;
}

// Order::~Order() {
//   while (!ordered_gears.empty()) {
//     auto gear = ordered_gears.back();
//     delete gear;
//     ordered_gears.pop();
//   }

//   for (auto gear : started_gears) {
//     delete gear;
//   }
// }

void Order::pushGear(Gear *gear) { this->ordered_gears.push(gear); }

Gear *Order::popGear() {
  std::unique_lock<std::mutex> ordered_lock(ordered_gears_mutex);
  Gear *gear = ordered_gears.front();
  this->ordered_gears.pop();
  ordered_lock.unlock();

  std::unique_lock<std::mutex> started_lock(started_gears_mutex);
  this->started_gears.push_back(gear);
  started_lock.unlock();

  return gear;
}

bool Order::gearsInQueue() { return ordered_gears.size() != 0; }

bool Order::readyToSend() {
  // gears waiting to start being processed
  if (gearsInQueue()) {
    return false;
  }

  // gears being processed
  std::unique_lock<std::mutex> lock(started_gears_mutex);
  for (auto gear : started_gears) {
    if (gear->getState() != GearState::PACKED) {
      lock.unlock();
      return false;
    }
  }

  if (lock.owns_lock()) {
    lock.unlock();
  }

  // all gears processed
  return true;
}

int Order::getID() { return ID; }

void Order::finalize() {
  for (auto gear : started_gears) {
    if (gear->state != GearState::PACKED) {
      Logger::log("Order_" + std::to_string(ID),
                  "ERROR - finalized order with unfinished gear.");
    }

    delete gear;
  }

  Logger::log("Order_" + std::to_string(ID), "Finalized.");
}
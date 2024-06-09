#include "../include/RobotB.h"
#include "../include/Factory.h"
#include "Logger.h"
#include <mutex>
#include <random>
#include <thread>

unsigned int RobotB::count = 0;

RobotB::RobotB(std::vector<GearStack *> stacks,
               std::vector<MillingMachine *> machines, Factory *factory)
    : stacks(stacks), machines(machines), factory(factory) {
  this->id = "RobotB" + std::to_string(count);
  count++;
  this->working = false;
  this->holding = nullptr;
}

std::string RobotB::getId() const { return this->id; }

bool RobotB::isHoldingGear() const { return holding != nullptr; }

void RobotB::workLoop() {
  std::random_device rd;
  std::mt19937 gen(rd());

  while (this->working) {
    // while (holding == nullptr) {
    //   if (!working) {
    //     return;
    //   }
      getGearFromStack();
    // }

    // transportation time
    std::uniform_int_distribution<> dis(1500, 3000);
    std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));

    // while (holding != nullptr) {
    //   if (!working) {
    //     return;
    //   }
      putGearIntoMachine();
    // }

    // transportation time
    std::uniform_int_distribution<> dis1(1500, 3000);
    std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
  }
}

void RobotB::getGearFromStack() {
  std::unique_lock<std::mutex> lock(factory->stacks_mutex);

  // wait if there are no non-full stacks
  factory->non_empty_stacks.wait(lock, [this] {
    for (auto stack : this->stacks) {
      if (!stack->full()) {
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

  for (auto stack : stacks) {
    if (!stack->empty()) {
      this->holding = stack->pop();
      lock.unlock();
      factory->non_empty_stacks.notify_one();

      Logger::log(this->id, "picked up a gear from " + stack->getId());
      return;
    }
  }

  // should never reach it, but just in case
  if (lock.owns_lock()) {
    lock.unlock();
  }
}

void RobotB::putGearIntoMachine() {
  std::unique_lock<std::mutex> lock(factory->machines_mutex);

  // wait if there are no empty machines
  factory->empty_machines.wait(lock, [this] {
    for (auto machine : this->machines) {
      if (machine->empty()) {
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

  // find empty machine and put the gear into it
  for (auto machine : machines) {
    if (machine->empty()) {
      Logger::log(this->id, "placed a gear in " + machine->getId());
      machine->startMilling(holding);
      this->holding = nullptr;
      lock.unlock();

      return;
    }
  }

  // should never reach it, but just in case
  if (lock.owns_lock()) {
    lock.unlock();
  }
}

std::thread RobotB::start() {
  if (!this->working) {
    this->working = true;
    std::thread worker(&RobotB::workLoop, this);
    Logger::log(this->id, "work loop started.");
    // worker.detach();
    return worker;
  }
}

void RobotB::stop() {
  this->working = false;
  Logger::log(this->id, "work stopped.");
}
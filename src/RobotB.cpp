#include "../include/RobotB.h"
#include "../include/Factory.h"
#include "Logger.h"
#include <mutex>

unsigned int RobotB::count = 0;

RobotB::RobotB(std::vector<GearStack *> stacks,
               std::vector<MillingMachine *> machines, Factory *factory)
    : stacks(stacks), machines(machines), factory(factory) {
  this->id = "RobotB" + std::to_string(count);
  count++;
  this->working = false;
  this->holding = nullptr;
}

std::string RobotB::getId() { return this->id; }

void RobotB::workLoop() {
  while (this->working) {
    while (holding == nullptr) {
      getGearFromStack();
    }

    while (holding != nullptr) {
      putGearIntoMachine();
    }
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

void RobotB::start() {
  if (!this->working) {
    this->working = true;
    std::thread worker(&RobotB::workLoop, this);
    Logger::log(this->id, "work loop started.");
    worker.detach();
  }
}

void RobotB::stop() {
  this->working = false;
  Logger::log(this->id, "work stopped.");
}
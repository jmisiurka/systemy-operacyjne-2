#include "../include/RobotA.h"
#include "../include/Factory.h"
#include "Logger.h"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <random>
#include <thread>

unsigned int RobotA::count = 0;

RobotA::RobotA(std::vector<GearStack *> stacks, std::vector<Furnace *> furnaces,
               Factory *factory)
    : stacks(stacks), furnaces(furnaces), factory(factory) {
  this->id = "RobotA" + std::to_string(count);
  count++;
  this->working = false;
  this->holding = nullptr;
}

std::string RobotA::getId() const { return this->id; }

bool RobotA::isHoldingGear() const { return holding != nullptr; }

void RobotA::workLoop() {
  std::random_device rd;
  std::mt19937 gen(rd());
  while (this->working) {
    // while (holding == nullptr) {
    getGearFromFurnace();
    // }

    // transportation time
    std::uniform_int_distribution<> dis(1500, 3000);
    std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));

    // while (holding != nullptr) {
    putGearOnStack();
    // }

    // transportation time
    std::uniform_int_distribution<> dis1(1500, 3000);
    std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
  }
}

void RobotA::getGearFromFurnace() {
  std::unique_lock<std::mutex> lock(factory->furnaces_mutex);

  // wait if there are no ready gears to take furnaces
  factory->finished_furnaces.wait(lock, [this] {
    for (auto furnace : this->furnaces) {
      if (furnace->finished()) {
        return true;
      }
    }

    return false;
  });

  // check if stop was demanded during waiting for a gear
  if (!this->working) {
    lock.unlock();
    return;
  }

  // dummy mutex and lock for waiting
  std::mutex m;
  std::unique_lock<std::mutex> l(m);
  // if power is off, wait until it turns on
  factory->power_on.wait(l, [this] { return factory->getPowerAvailable(); });

  // search through furnaces to find a finished one
  for (auto furnace : furnaces) {
    if (furnace->finished()) {
      this->holding = furnace->getGear();
      lock.unlock();
      Logger::log(this->id, "picked up a gear from " + furnace->getId());
      factory->empty_furnaces.notify_one();

      return;
    }
  }

  // should never reach it, but just in case
  if (lock.owns_lock()) {
    lock.unlock();
  }
}

void RobotA::putGearOnStack() {
  std::unique_lock<std::mutex> lock(factory->stacks_mutex);

  // wait if there are no non-full stacks
  factory->non_full_stacks.wait(lock, [this] {
    for (auto stack : this->stacks) {
      if (!stack->full()) {
        return true;
      }
    };

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
    if (!stack->full()) {
      stack->push(holding);
      this->holding = nullptr;
      Logger::log(this->id, "placed a gear on " + stack->getId());

      lock.unlock();
      factory->non_empty_stacks.notify_one();
      return;
    }
  }

  // should never reach it, but just in case
  if (lock.owns_lock()) {
    lock.unlock();
  }
}

std::thread RobotA::start() {
  if (!this->working) {
    this->working = true;
    std::thread worker(&RobotA::workLoop, this);
    Logger::log(this->id, "work loop started.");
    return worker;
  }
}

void RobotA::stop() {
  this->working = false;
  Logger::log(this->id, "work stopped.");
}
#include "../include/MillingMachine.h"
#include "../include/Factory.h"
#include "Logger.h"
#include <thread>
#include <random>
#include <string>

unsigned int MillingMachine::count = 0;

MillingMachine::MillingMachine(Factory *factory) {
  this->factory = factory;
  this->gear = nullptr;
  this->id = "Machine" + std::to_string(this->count);
  this->count++;
}

std::string MillingMachine::getId() const { return this->id; }

bool MillingMachine::finished() const {
  if (this->gear != nullptr && this->gear->getState() == GearState::MILLED) {
    return true;
  }

  return false;
}

bool MillingMachine::empty() const { return this->gear == nullptr; }

void MillingMachine::startMilling(Gear *gear) {
  if (gear != nullptr) {
    this->gear = gear;
    std::thread worker(&MillingMachine::mill, this);
    Logger::log(this->id, "started milling.");

    worker.detach();
  }
}

void MillingMachine::mill() {
  gear->setState(GearState::MILLING);

  std::random_device rd;
  std::mt19937 gen(rd());

  std::uniform_int_distribution<> dis(5000, 10000);

  std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));

  factory->finished_machines.notify_one();
  Logger::log(this->id, "finished milling.");

  gear->setState(MILLED);
}

Gear *MillingMachine::removeGear() {
  Gear *gear = this->gear;
  this->gear = nullptr;
  return gear;
}
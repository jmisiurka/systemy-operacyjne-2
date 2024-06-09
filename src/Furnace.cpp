#include "Furnace.h"
#include "Factory.h"
#include "Logger.h"
#include <chrono>
#include <random>
#include <thread>

unsigned int Furnace::count = 0;

Furnace::Furnace(Factory *factory) {
  this->factory = factory;
  this->gear = nullptr;
  this->id = "Furnace" + std::to_string(count);
  Furnace::count++;
}

std::string Furnace::getId() { return this->id; }

bool Furnace::finished() {
  if (this->gear != nullptr && this->gear->state == CASTED) {
    return true;
  }

  return false;
}

bool Furnace::empty() { return this->gear == nullptr; }

bool Furnace::busy() { return !this->empty() && !this->finished(); }

Gear *Furnace::getGear() {
  Gear *gear = this->gear;
  this->gear = nullptr;
  return gear;
}

void Furnace::startCasting(Gear *gear) {
  if (gear != nullptr) {
    this->gear = gear;
    std::thread worker(&Furnace::cast, this);
    Logger::log(this->id, "started casting.");

    worker.detach();
  }

  return;
}

// runs on a dedicated thread
void Furnace::cast() {
  this->gear->setState(GearState::CASTING);

  std::random_device rd;
  std::mt19937 gen(rd());

  std::uniform_int_distribution<> dis(3000, 6000);

  std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
  Logger::log(this->id, "finished casting.");

  gear->setState(CASTED);

  // notify waiting threads that there is a finished furnace waiting to be
  // emptied
  factory->finished_furnaces.notify_all();
}

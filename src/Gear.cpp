#include "../include/Gear.h"
#include <mutex>

Gear::Gear() { this->state = GearState::ORDERED; }

GearState Gear::getState() { return this->state; }

void Gear::setState(GearState newState) {
  std::unique_lock<std::mutex> lock(state_mutex);
  this->state = newState;
  lock.unlock();
}
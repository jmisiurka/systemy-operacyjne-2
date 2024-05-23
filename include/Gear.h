#pragma once

#include <mutex>
enum GearState { ORDERED, CASTING, CASTED, MILLING, MILLED, PACKED };

struct Gear {
  std::mutex state_mutex;
  GearState state;

public:
  Gear();

  GearState getState();
  void setState(GearState newState);
};
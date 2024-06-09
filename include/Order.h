#pragma once
#include "Gear.h"
#include <mutex>
#include <vector>

class Order {
  std::mutex ordered_gears_mutex;
  std::mutex started_gears_mutex;

  std::vector<Gear *> started_gears;
  std::vector<Gear *> ordered_gears;
  static int count;
  int ID;

public:
  Order();
  Order(const Order &order);
  Order &operator=(const Order &&order);
  Order(std::vector<Gear *> ordered_gears);
  // ~Order();

  void pushGear(Gear *);
  Gear *popGear();
  bool gearsInQueue();
  bool readyToSend();

  int getID() const;
  int getTotalOrderSize() const;
  int getFinishedGearsAmount();
  void finalize();
};
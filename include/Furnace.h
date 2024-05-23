#pragma once

#include "Gear.h"
#include <string>

class Factory;

class Furnace {
  Factory *factory;

  static unsigned int count;
  std::string id;
  Gear *gear;

  void cast();

public:
  Furnace(Factory *factory);

  std::string getId();
  bool empty();
  bool finished();
  bool busy();
  Gear *getGear();
  void startCasting(Gear *gear);
};
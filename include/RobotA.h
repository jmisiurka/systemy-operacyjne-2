#pragma once

#include "Furnace.h"
#include "Gear.h"
#include "GearStack.h"
#include <string>
#include <thread>
#include <vector>

class Factory;

class RobotA {
  static unsigned int count;

  std::string id;
  bool working;
  const std::vector<GearStack *> stacks;
  const std::vector<Furnace *> furnaces;
  Factory *factory;
  Gear *holding;

  void workLoop();
  void getGearFromFurnace();
  void putGearOnStack();

public:
  RobotA(std::vector<GearStack *> stacks, std::vector<Furnace *> furnaces,
         Factory *factory);

  std::string getId() const;
  bool isHoldingGear() const;

  std::thread start();
  void stop();
};
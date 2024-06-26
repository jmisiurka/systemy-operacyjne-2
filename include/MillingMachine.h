#ifndef MILLING_MACHINE_H_
#define MILLING_MACHINE_H_

#include "Furnace.h"
#include "Gear.h"
#include <string>

class MillingMachine {
  Factory *factory;

  static unsigned int count;
  std::string id;
  Gear *gear;

  void mill();

public:
  MillingMachine(Factory *factory);

  std::string getId() const;

  bool finished() const;
  bool empty() const;
  bool busy() const;
  void startMilling(Gear *gear);
  Gear *removeGear();
};

#endif

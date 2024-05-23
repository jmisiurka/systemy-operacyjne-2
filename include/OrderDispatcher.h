#pragma once

#include "Furnace.h"
#include "MillingMachine.h"
#include "Order.h"
#include <mutex>
#include <string>

class Factory;

class OrderDispatcher {
  std::vector<Order> orders;
  static const std::string id;
  bool working;

  std::vector<Furnace *> &furnaces;
  std::vector<MillingMachine *> &machines;
  Factory *factory;

  std::mutex orders_mutex;

  void incomingOrdersLoop();
  void outgoingOrdersLoop();
  void handleFinishedGears();
  void startProcess();
  void packGear();

public:
  OrderDispatcher(std::vector<Furnace *> &furnaces,
                  std::vector<MillingMachine *> &machines, Factory *factory);

  void start();
  void stop();
  void addOrder(Order order);

  std::mutex &getOrderMutex();
  int getQueueLength();
};
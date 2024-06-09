#pragma once

#include "Furnace.h"
#include "MillingMachine.h"
#include "Order.h"
#include <mutex>
#include <string>

class Factory;

class OrderDispatcher {
  std::vector<Order *> orders;
  int finished_orders = 0;
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

  friend class Display;

public:
  OrderDispatcher(std::vector<Furnace *> &furnaces,
                  std::vector<MillingMachine *> &machines, Factory *factory);
  ~OrderDispatcher();

  void start();
  void stop();
  void addOrder(Order *order);

  // std::mutex &getOrderMutex();
  int getQueueLength() const;
  int getFinishedOrders() const;
};
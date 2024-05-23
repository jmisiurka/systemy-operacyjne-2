#include "../include/Factory.h"
#include "Order.h"
#include "OrderDispatcher.h"
#include <ncurses.h>
#include <thread>

Factory::Factory(int n_stacks, int n_furnaces, int n_machines, int n_robots_a,
                 int n_robots_b)
    : orderDispatcher(OrderDispatcher(this->furnaces, this->machines, this)) {
  for (int i = 0; i < n_stacks; ++i) {
    this->stacks.push_back(new GearStack());
  }

  for (int i = 0; i < n_furnaces; ++i) {
    this->furnaces.push_back(new Furnace(this));
  }

  for (int i = 0; i < n_machines; ++i) {
    this->machines.push_back(new MillingMachine(this));
  }

  for (int i = 0; i < n_robots_a; ++i) {
    this->robots_a.push_back(new RobotA(this->stacks, this->furnaces, this));
  }

  for (int i = 0; i < n_robots_b; ++i) {
    this->robots_b.push_back(new RobotB(this->stacks, this->machines, this));
  }

  this->power = false;
}

Factory::~Factory() {
  for (auto stack : stacks) {
    delete stack;
  }

  for (auto furnace : furnaces) {
    delete furnace;
  }

  for (auto machine : machines) {
    delete machine;
  }

  for (auto robot : robots_a) {
    delete robot;
  }

  for (auto robot : robots_b) {
    delete robot;
  }
}

void Factory::run() {
  this->power = true;
  for (auto robot : robots_a) {
    robot->start();
  }

  for (auto robot : robots_b) {
    robot->start();
  }

  std::thread orderGeneratorThread(&Factory::orderGenerator, this);
  orderDispatcher.start();

  orderGeneratorThread.join();
}

void Factory::inputListener() {
  while (true) {
    // TODO - DODAĆ ZANIK PRĄDU
  }
}

void Factory::orderGenerator() {
  while (true) {

    // max 5 orders waiting at once
    while (orderDispatcher.getQueueLength() > 5)
      ;

    Order order = Order();

    // make it random in some way
    for (int i = 0; i < 3; i++) {
      order.pushGear(new Gear());
    }

    // TODO some move operator???
    orderDispatcher.addOrder(order);
  }
}
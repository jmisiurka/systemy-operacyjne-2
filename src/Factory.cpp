#include "../include/Factory.h"
#include "Display.h"
#include "Order.h"
#include "OrderDispatcher.h"
#include "Logger.h"
#include <cstdio>
#include <ncurses.h>
#include <random>
#include <thread>

Factory::Factory(int n_stacks, int n_furnaces, int n_machines, int n_robots_a,
                 int n_robots_b)
    : orderDispatcher(OrderDispatcher(this->furnaces, this->machines, this)),
      display(Display(this->stacks, this->furnaces, this->machines,
                      this->robots_a, this->robots_b, &this->orderDispatcher,
                      this)) {

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

  this->power.store(false);

  this->display.init();
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

  endwin();
}

void Factory::run() {
  this->power = true;
  for (auto robot : robots_a) {
    machine_threads.push_back(robot->start());
  }

  for (auto robot : robots_b) {
    machine_threads.push_back(robot->start());
  }

  orderDispatcher.start();

  std::thread orderGeneratorThread(&Factory::orderGenerator, this);
  std::thread input(&Factory::inputListener, this);

  orderGeneratorThread.join();
  input.join();
}

void Factory::inputListener() {
  while (true) {
    char c = getc(stdin);
    switch (c) {
    case 'p':
      if (!power.load()) {
        power.store(true);
        power_on.notify_all();
      } else {
        power.store(false);
      }
      break;
    case 'x':
      shutdown();
      return;
    }
  }
}

void Factory::shutdown() {
  Logger::log("Factory", "shutdown started.");
  
  for (auto robot : robots_a) {
    robot->stop();
  }

  for (auto robot : robots_b) {
    robot->stop();
  }

  for (auto &thread : machine_threads) {
    thread.join();
  }

  for (auto &furnace : furnaces) {
    while (furnace->busy())
      ;
  }

  for (auto &machine : machines) {
    // wait for machines to stop
    while (machine->busy())
      ;
  }

  orderDispatcher.stop();

  display.stop();
}

void Factory::orderGenerator() {
  std::random_device rd;
  std::mt19937 gen(rd());
  while (true) {

    // max 5 orders waiting at once
    while (orderDispatcher.getQueueLength() >= 10)
      ;

    Order *order = new Order();

    // transportation time
    std::uniform_int_distribution<> dis(3, 5);

    // make it random in some way
    int amount = dis(gen);
    for (int i = 0; i < amount; i++) {
      order->pushGear(new Gear());
    }

    orderDispatcher.addOrder(order);
  }
}

bool Factory::getPowerAvailable() { return power.load(); }
#ifndef FACTORY_H_
#define FACTORY_H_

#include "Display.h"
#include "Furnace.h"
#include "GearStack.h"
#include "MillingMachine.h"
#include "OrderDispatcher.h"
#include "RobotA.h"
#include "RobotB.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

class Factory {
  Display display;

  std::vector<std::thread> machine_threads;

  // factory equipment
  std::vector<GearStack *> stacks;
  std::vector<Furnace *> furnaces;
  std::vector<MillingMachine *> machines;
  std::vector<RobotA *> robots_a;
  std::vector<RobotB *> robots_b;

  OrderDispatcher orderDispatcher;

  std::atomic<bool> power;

public:
  Factory(int n_stacks = 3, int n_furnaces = 3, int n_machines = 3,
          int n_robots_a = 2, int n_robots_b = 2);
  ~Factory();
  void run();
  void inputListener();
  void orderGenerator();
  bool getPowerAvailable();
  void shutdown();

  std::condition_variable finished_furnaces;
  std::condition_variable empty_furnaces;
  std::condition_variable non_empty_stacks;
  std::condition_variable non_full_stacks;
  std::condition_variable empty_machines;
  std::condition_variable finished_machines;

  std::condition_variable_any power_on;

  std::mutex furnaces_mutex;
  std::mutex stacks_mutex;
  std::mutex machines_mutex;
};

#endif

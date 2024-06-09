#pragma once
#include "Furnace.h"
#include "GearStack.h"
#include "MillingMachine.h"
#include "OrderDispatcher.h"
#include "RobotA.h"
#include "RobotB.h"
#include <ncurses.h>
#include <queue>
#include <vector>

class Factory;

class Display {
  WINDOW *status_border;
  WINDOW *status;
  WINDOW *orders_border;
  WINDOW *orders;
  WINDOW *main_border;
  WINDOW *main;
  WINDOW *logs_border;
  WINDOW *logs;

  std::mutex log_mutex;

  std::queue<std::string> log_queue;

  int screen_width;

  const std::vector<GearStack *> &stacks;
  const std::vector<Furnace *> &furnaces;
  const std::vector<MillingMachine *> &machines;
  const std::vector<RobotA *> &robots_a;
  const std::vector<RobotB *> &robots_b;
  OrderDispatcher *dispatcher;
  Factory *factory;

  bool working;

  void update_loop();

public:
  Display(std::vector<GearStack *> &stacks, std::vector<Furnace *> &furnaces,
          std::vector<MillingMachine *> &machines,
          std::vector<RobotA *> &robots_a, std::vector<RobotB *> &robots_b,
          OrderDispatcher *dispatcher, Factory *factory);

  void init();
  void log(std::string id, std::string msg);

  void stop();
};
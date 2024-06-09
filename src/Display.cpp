#include "Display.h"
#include "Factory.h"
#include "Logger.h"
#include <chrono>
#include <mutex>
#include <ncurses.h>
#include <string>
#include <thread>

Display::Display(std::vector<GearStack *> &stacks,
                 std::vector<Furnace *> &furnaces,
                 std::vector<MillingMachine *> &machines,
                 std::vector<RobotA *> &robots_a,
                 std::vector<RobotB *> &robots_b, OrderDispatcher *dispatcher,
                 Factory *factory)
    : stacks(stacks), furnaces(furnaces), machines(machines),
      robots_a(robots_a), robots_b(robots_b), dispatcher(dispatcher),
      factory(factory) {}

void draw_furnace(WINDOW *win, int offset_x, int offset_y, Furnace *furnace);
void draw_robot_a(WINDOW *win, int offset_x, int offset_y, RobotA *robot);
void draw_robot_b(WINDOW *win, int offset_x, int offset_y, RobotB *robot);
void draw_stack(WINDOW *win, int offset_x, int offset_y, GearStack *stack);
void draw_machine(WINDOW *win, int offset_x, int offset_y,
                  MillingMachine *machine);
void print_order_info(WINDOW *win, int offset_x, int offset_y, Order *order);

void Display::init() {
  Logger::initDisplay(this);

  std::unique_lock<std::mutex> lock(log_mutex);

  initscr(); // initialize ncurses view
  cbreak();  // disable input buffering
  noecho();  // disable echo
  screen_width = getmaxx(stdscr);

  // colors
  start_color();
  init_color(8, 300, 300, 300);
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, 8, COLOR_BLACK);
  init_pair(3, COLOR_GREEN, COLOR_BLACK);
  init_pair(4, COLOR_RED, COLOR_BLACK);

  // status window
  status_border = newwin(5, screen_width, 0, 0);
  box(status_border, 0, 0);
  wattron(status_border, A_BOLD);
  mvwprintw(status_border, 0, 1, "STATUS");
  wattroff(status_border, A_BOLD);
  wrefresh(status_border);
  status = newwin(getmaxy(status_border) - 2, screen_width - 2,
                  getbegy(status_border) + 1, getbegx(status_border) + 1);
  wrefresh(status);

  // orders window
  orders_border = newwin(5, screen_width,
                         getbegy(status_border) + getmaxy(status_border), 0);
  box(orders_border, 0, 0);
  wattron(orders_border, A_BOLD);
  mvwprintw(orders_border, 0, 1, "ORDERS");
  wattroff(orders_border, A_BOLD);
  wrefresh(orders_border);
  orders = newwin(getmaxy(orders_border) - 2, screen_width - 2,
                  getbegy(orders_border) + 1, getbegx(orders_border) + 1);
  wrefresh(orders);

  // main window
  main_border = newwin(25, screen_width,
                       getbegy(orders_border) + getmaxy(orders_border), 0);
  box(main_border, 0, 0);
  wattron(main_border, A_BOLD);
  mvwprintw(main_border, 0, 1, "FACTORY");
  wattroff(main_border, A_BOLD);
  wrefresh(main_border);
  main = newwin(getmaxy(main_border) - 2, screen_width - 2,
                getbegy(main_border) + 1, getbegx(main_border) + 1);
  wrefresh(main);

  // // logs window
  logs_border =
      newwin(10, screen_width, getbegy(main_border) + getmaxy(main_border), 0);
  box(logs_border, 0, 0);
  wattron(logs_border, A_BOLD);
  mvwprintw(logs_border, 0, 1, "LOGS");
  wattroff(logs_border, A_BOLD);
  wrefresh(logs_border);
  logs = newwin(getmaxy(logs_border) - 2, screen_width - 2,
                getbegy(logs_border) + 1, getbegx(logs_border) + 1);
  scrollok(logs, TRUE);
  wrefresh(logs);

  working = true;

  std::thread loop(&Display::update_loop, this);
  loop.detach();

  lock.unlock();
}

void Display::update_loop() {
  bool prev_power = !factory->getPowerAvailable();
  int i = 0;
  do {
    //---------STATUS WINDOW----------
    // power status
    // lock.lock();
    bool power = factory->getPowerAvailable();
    if (power != prev_power) {
      if (factory->getPowerAvailable()) {
        mvwprintw(status, 0, 1, "Power ON ");
      } else {

        wattron(status, A_BLINK | COLOR_PAIR(4));
        mvwprintw(status, 0, 1, "Power OFF");
        wattroff(status, A_BLINK | COLOR_PAIR(4));
      }
    }
    prev_power = power;

    // finished orders counter
    mvwprintw(status, 1, 1, "Finished orders: %d",
              dispatcher->getFinishedOrders());

    // orders in queue counter
    mvwprintw(status, 2, 1, "Orders in queue: %d/10",
              dispatcher->getQueueLength());

    //----------ORDERS WINDOW-----------
    // dispatcher->orders_mutex.lock();
    for (int i = 0; i < dispatcher->getQueueLength(); i++) {
      Order* order = dispatcher->orders[i];
      print_order_info(orders, 10 * i, 0, order);
    }
    // dispatcher->orders_mutex.unlock();

    //----------FACTORY WINDOW----------
    // furnaces
    for (int i = 0; i < furnaces.size(); i++) {
      draw_furnace(main, 5, 9 + 7 * (i - 1), furnaces[i]);
    }

    for (int i = 0; i < robots_a.size(); i++) {
      draw_robot_a(main, 25, 12 + 10 * (i - 1), robots_a[i]);
    }

    for (int i = 0; i < stacks.size(); i++) {
      draw_stack(main, 47, 8 + 5 * (i - 1), stacks[i]);
    }

    for (int i = 0; i < robots_b.size(); i++) {
      draw_robot_b(main, 75, 12 + 10 * (i - 1), robots_b[i]);
    }

    for (int i = 0; i < machines.size(); i++) {
      draw_machine(main, 100, 9 + 7 * (i - 1), machines[i]);
    }

    //----------LOGS WINDOW-------------
    log_mutex.lock();
    while (!log_queue.empty()) {
      std::string s = log_queue.front().c_str();
      wprintw(logs, "%s\n", log_queue.front().c_str());
      wrefresh(logs);
      log_queue.pop();
    }
    log_mutex.unlock();

    // refresh all windows
    wrefresh(status);
    wrefresh(orders);
    wrefresh(main);

    // refreshing delay
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  } while (working);

  wprintw(logs, "Stopped, CTRL+C to quit.");
  wrefresh(logs);
}

void Display::log(std::string id, std::string msg) {
  log_mutex.lock();
  log_queue.push(("[" + id + "]: " + msg));
  log_mutex.unlock();
}

void Display::stop() { working = false; }

void draw_furnace(WINDOW *win, int offset_x, int offset_y, Furnace *furnace) {
  bool has_gear = !furnace->empty();
  bool finished = furnace->finished();
  int color_pair = 2;
  if (finished) {
    color_pair = 3;
  } else if (has_gear) {
    color_pair = 1;
  }

  // Draw the outer box of the furnace
  wattron(win, COLOR_PAIR(color_pair));
  mvwprintw(win, offset_y, offset_x, "%s", furnace->getId().c_str());
  mvwprintw(win, offset_y + 1, offset_x, "#####");
  mvwprintw(win, offset_y + 2, offset_x, "#   #");
  mvwprintw(win, offset_y + 3, offset_x, "# %c #", has_gear ? 'o' : ' ');
  mvwprintw(win, offset_y + 4, offset_x, "#   #");
  mvwprintw(win, offset_y + 5, offset_x, "#####");
  wattroff(win, COLOR_PAIR(color_pair));

  // Refresh the screen to display changes
  // wrefresh(win);
}

void draw_robot_a(WINDOW *win, int offset_x, int offset_y, RobotA *robot) {
  bool has_gear = robot->isHoldingGear();
  int color_pair = has_gear ? 1 : 2;

  wattron(win, COLOR_PAIR(color_pair));
  mvwprintw(win, offset_y, offset_x, "%s", robot->getId().c_str());
  mvwprintw(win, offset_y + 1, offset_x, "     ( )      ");
  mvwprintw(win, offset_y + 2, offset_x, "     / \\      ");
  mvwprintw(win, offset_y + 3, offset_x, "    /   \\     ");
  mvwprintw(win, offset_y + 4, offset_x, "   /    ( )   ");
  mvwprintw(win, offset_y + 5, offset_x, " _/_    _|_   ");
  mvwprintw(win, offset_y + 6, offset_x, "/ A \\  | %c |  ",
            has_gear ? 'o' : ' ');
  mvwprintw(win, offset_y + 7, offset_x, "|___|         ");
  wattroff(win, COLOR_PAIR(color_pair));

  // Refresh the screen to display changes
  // wrefresh(win);
}

void draw_robot_b(WINDOW *win, int offset_x, int offset_y, RobotB *robot) {
  bool has_gear = robot->isHoldingGear();
  int color_pair = has_gear ? 1 : 2;

  wattron(win, COLOR_PAIR(color_pair));
  mvwprintw(win, offset_y, offset_x, "%s", robot->getId().c_str());
  mvwprintw(win, offset_y + 1, offset_x, "     ( )      ");
  mvwprintw(win, offset_y + 2, offset_x, "     / \\      ");
  mvwprintw(win, offset_y + 3, offset_x, "    /   \\     ");
  mvwprintw(win, offset_y + 4, offset_x, "   /    ( )   ");
  mvwprintw(win, offset_y + 5, offset_x, " _/_    _|_   ");
  mvwprintw(win, offset_y + 6, offset_x, "/ B \\  | %c |  ",
            has_gear ? 'o' : ' ');
  mvwprintw(win, offset_y + 7, offset_x, "|___|         ");
  wattroff(win, COLOR_PAIR(color_pair));

  // Refresh the screen to display changes
  // wrefresh(win);
}

void draw_stack(WINDOW *win, int offset_x, int offset_y, GearStack *stack) {
  int gearCount = stack->getAmount();

  // Draw the outer box of the furnace
  mvwprintw(win, offset_y, offset_x, "%s", stack->getId().c_str());
  mvwprintw(
      win, offset_y + 1, offset_x, "%s",
      (std::string(gearCount, 'o') + std::string(5 - gearCount, ' ')).c_str());
  mvwprintw(win, offset_y + 2, offset_x, "#####");

  // Refresh the screen to display changes
  // wrefresh(win);
}

void draw_machine(WINDOW *win, int offset_x, int offset_y,
                  MillingMachine *machine) {
  bool has_gear = !machine->empty();
  bool finished = machine->finished();
  int color_pair = 2;
  if (finished) {
    color_pair = 3;
  } else if (has_gear) {
    color_pair = 1;
  }
  // Draw the outer box of the furnace
  wattron(win, COLOR_PAIR(color_pair));
  mvwprintw(win, offset_y, offset_x, "%s", machine->getId().c_str());
  mvwprintw(win, offset_y + 1, offset_x, "#####");
  mvwprintw(win, offset_y + 2, offset_x, "#   #");
  mvwprintw(win, offset_y + 3, offset_x, "# %c #", has_gear ? 'o' : ' ');
  mvwprintw(win, offset_y + 4, offset_x, "#   #");
  mvwprintw(win, offset_y + 5, offset_x, "#####");
  wattroff(win, COLOR_PAIR(color_pair));

  // Refresh the screen to display changes
  // wrefresh(win);
}

void print_order_info(WINDOW *win, int offset_x, int offset_y, Order *order) {
  mvwprintw(win, offset_y, offset_x, "Order %d", order->getID());

  mvwprintw(win, offset_y + 1, offset_x, "%d/%d", order->getFinishedGearsAmount(), order->getTotalOrderSize());
}
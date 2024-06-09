#pragma once

#include "Display.h"
#include <string>

class Logger {
  static Display* display;

public:
  static void log(std::string caller_id, std::string message);

  static void initDisplay(Display *factory_display);
};
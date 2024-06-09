#include "Logger.h"

Display *Logger::display = nullptr;

void Logger::log(std::string caller_id, std::string message) {
  display->log(caller_id, message);
}

void Logger::initDisplay(Display *factory_display) {
  Logger::display = factory_display;
}
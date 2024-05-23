#include "../include/GearStack.h"
#include <string>

unsigned int GearStack::count = 0;

GearStack::GearStack() {
  this->id = "Stack" + std::to_string(count);
  count++;
}

std::string GearStack::getId() { return this->id; }

bool GearStack::empty() { return this->stack.empty(); }

bool GearStack::full() { return this->stack.size() == 5; }

Gear *GearStack::pop() {
  Gear *gear = this->stack.top();
  this->stack.pop();
  return gear;
}

void GearStack::push(Gear *gear) { this->stack.push(gear); }

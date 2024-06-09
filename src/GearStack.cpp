#include "../include/GearStack.h"
#include <string>

unsigned int GearStack::count = 0;

GearStack::GearStack() {
  this->id = "Stack" + std::to_string(count);
  count++;
}

std::string GearStack::getId() const { return this->id; }

int GearStack::getAmount() const { return stack.size(); }

bool GearStack::empty() const { return this->stack.empty(); }

bool GearStack::full() const { return this->stack.size() == 5; }

Gear *GearStack::pop() {
  Gear *gear = this->stack.top();
  this->stack.pop();
  return gear;
}

void GearStack::push(Gear *gear) { this->stack.push(gear); }

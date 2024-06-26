#pragma once

#include "Gear.h"
#include <string>
#include <stack>

#define SIZE 5

class GearStack
{
    static unsigned int count;

    std::string id;

    std::stack<Gear*> stack;
public:
    GearStack();

    std::string getId() const;
    int getAmount() const;
    bool empty()const;
    bool full()const;
  
    Gear* pop();
    void push(Gear*);
};
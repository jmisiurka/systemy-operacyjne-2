#pragma once

#include "Gear.h"
#include <mutex>
#include <stack>

#define SIZE 5

class GearStack
{
    static unsigned int count;

    std::string id;

    int current_amount;
    std::stack<Gear*> stack;
public:
    GearStack();

    std::string getId();

    bool empty();
    bool full();
    Gear* pop();
    void push(Gear*);
};
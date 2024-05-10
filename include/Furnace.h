#pragma once

#include "Gear.h"
#include "Logger.h"
#include <string>
#include <thread>
#include <chrono>
#include <iostream>


class Furnace {
    static unsigned int count;
    std::string id;
    Gear* gear;

    void cast();
public:
    Furnace();

    std::string getId();
    bool empty();
    bool finished();
    Gear* getGear();
    void startCasting(Gear* gear);
};
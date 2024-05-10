#pragma once

#include "Gear.h"
#include "Logger.h"
#include <string>
#include <thread>

class MillingMachine {
    static unsigned int count;
    std::string id;
    Gear *gear;

    void mill();
public:
    MillingMachine();

    std::string getId();

    bool finished();
    bool empty();
    void startMilling(Gear *gear);
};
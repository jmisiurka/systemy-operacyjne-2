#pragma once

#include "GearStack.h"
#include "MillingMachine.h"
#include <vector>
#include <thread>
#include <iostream>

class Factory;

class RobotB {
    static unsigned int count;

    std::string id;
    bool working;
    std::vector<GearStack> &stacks;
    std::vector<MillingMachine> &machines;
    Factory* factory;
    Gear* holding;

    void workLoop();
    void getGearFromStack();
    void putGearIntoMachine();

public:
    RobotB(std::vector<GearStack> &stacks, std::vector<MillingMachine> &machines);

    std::string getId();

    void start();
    void stop();
};
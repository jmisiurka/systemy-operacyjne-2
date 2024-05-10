#pragma once

#include "Gear.h"
#include "GearStack.h"
#include "Furnace.h"
#include <vector>
#include <thread>
#include <iostream>
#include <mutex>
#include <chrono>
#include <string>

class Factory;

class RobotA
{
    static unsigned int count;

    std::string id;
    bool working;
    std::vector<GearStack> &stacks;
    std::vector<Furnace> &furnaces;
    Factory* factory;
    Gear* holding;

    void workLoop();
    void getGearFromFurnace();
    void putGearOnStack();

public:
    RobotA(std::vector<GearStack> &stacks, std::vector<Furnace> &furnaces);

    std::string getId();

    void start();
    void stop();
};
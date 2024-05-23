#pragma once

#include "Gear.h"
#include "GearStack.h"
#include "Furnace.h"
#include <vector>
#include <string>

class Factory;

class RobotA
{
    static unsigned int count;

    std::string id;
    bool working;
    const std::vector<GearStack*> stacks;
    const std::vector<Furnace*> furnaces;
    Factory* factory;
    Gear* holding;

    void workLoop();
    void getGearFromFurnace();
    void putGearOnStack();

public:
    RobotA(std::vector<GearStack*> stacks, std::vector<Furnace*> furnaces, Factory* factory);

    std::string getId() const;

    void start();
    void stop();
};
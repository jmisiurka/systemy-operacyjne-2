#pragma once

#include "Furnace.h"
#include "MillingMachine.h"
#include "GearStack.h"
#include "RobotA.h"
#include "RobotB.h"
#include <mutex>
#include <vector>
#include <thread>

class Factory {
    // factory equipment
    std::vector<GearStack> stacks;
    std::vector<Furnace> furnaces;
    std::vector<MillingMachine> machines;
    std::vector<RobotA> robots_a;
    std::vector<RobotB> robots_b;

public:
    Factory(int n_stacks = 3, int n_furnaces = 3, int n_machines = 3, int n_robots_a = 2, int n_robots_b = 2);
    void run();
    std::mutex furnaces_lock;
    std::mutex stacks_lock;
    std::mutex machines_lock;
};
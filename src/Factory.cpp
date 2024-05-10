#include "../include/Factory.h"

Factory::Factory(int n_stacks, int n_furnaces, int n_machines, int n_robots_a, int n_robots_b)
{
    for (int i = 0; i < n_stacks; ++i)
    {
        this->stacks.push_back(GearStack());
    }

    for (int i = 0; i < n_furnaces; ++i)
    {
        this->furnaces.push_back(Furnace());
    }

    for (int i = 0; i < n_machines; ++i)
    {
        this->machines.push_back(MillingMachine());
    }

    for (int i = 0; i < n_robots_a; ++i)
    {
        this->robots_a.push_back(RobotA(this->stacks, this->furnaces));
    }

    for (int i = 0; i < n_robots_b; ++i)
    {
        this->robots_b.push_back(RobotB(this->stacks, this->machines));
    }
}



void Factory::run()
{
    for (RobotA robot : robots_a)
    {
        robot.start();
    }

    for (RobotB robot : robots_b)
    {
        robot.start();
    }
}

//TODO - DODAĆ ZANIK PRĄDU
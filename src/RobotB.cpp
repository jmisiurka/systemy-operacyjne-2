#include "../include/RobotB.h"
#include "../include/Factory.h"

unsigned int RobotB::count = 0;

RobotB::RobotB(std::vector<GearStack> &stacks, std::vector<MillingMachine> &machines): stacks(stacks), machines(machines)
{
    this->id = "RobotB" + std::to_string(count);
    count++;
    this->working = false;
    this->holding = nullptr;
}

std::string RobotA::getId() {
    return this->id;
}

void RobotB::workLoop()
{
    while (this->working)
    {
        while (holding == nullptr)
        {
            getGearFromStack();
        }

        while (holding != nullptr)
        {
            putGearIntoMachine();
        }
    }
}

void RobotB::getGearFromStack()
{
    factory->stacks_lock.lock();

    for (GearStack stack : stacks)
    {
        if (!stack.empty())
        {
            this->holding = stack.pop();
            Logger::log(this->id, "picked up a gear from " + stack.getId());
            factory->stacks_lock.unlock();
            return;
        }
    }

    factory->stacks_lock.unlock();

    // sleep before returning and trying another attempt
    // TODO - zmienna warunkowa
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void RobotB::putGearIntoMachine()
{
    factory->machines_lock.lock();
    
    for (MillingMachine machine : machines)
    {
        if (machine.empty())
        {
            machine.startMilling(holding);
            this->holding = nullptr;
            Logger::log(this->id, "placed a gear in " + machine.getId());
            factory->machines_lock.unlock();
            return;
        }
    }

    factory->machines_lock.unlock();

    // sleep before returning and trying another attempt
    // TODO - zmienna warunkowa
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void RobotB::start()
{
    if (!this->working)
    {
        this->working = true;
        std::thread worker(&RobotB::workLoop, this);
        Logger::log(this->id, "work loop started.");
    }
}

void RobotB::stop()
{
    this->working = false;
    Logger::log(this->id, "work stopped.");
}
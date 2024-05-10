#include "../include/RobotA.h"
#include "../include/Factory.h"

unsigned int RobotA::count = 0;

RobotA::RobotA(std::vector<GearStack> &stacks, std::vector<Furnace> &furnaces): stacks(stacks), furnaces(furnaces)
{
    this->id = "RobotA" + std::to_string(count);
    count++;
    this->working = false;
    this->holding = nullptr;
}

std::string RobotA::getId() {
    return this->id;
}

void RobotA::workLoop()
{
    while (this->working)
    {
        while (holding == nullptr)
        {
            getGearFromFurnace();
        }

        while (holding != nullptr)
        {
            putGearOnStack();
        }
    }
}

void RobotA::getGearFromFurnace()
{
    factory->furnaces_lock.lock();

    for (Furnace furnace : furnaces)
    {
        if (furnace.finished())
        {
            this->holding = furnace.getGear();  // start molding another gear as well
            Logger::log(this->id, "picked up a gear from " + furnace.getId());
            factory->furnaces_lock.unlock();
            return;
        }
    }

    factory->furnaces_lock.unlock();

    // sleep before returning and trying another attempt
    // TODO - zmienna warunkowa
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void RobotA::putGearOnStack()
{
    factory->stacks_lock.lock();
    
    for (GearStack stack : stacks)
    {       
        if (!stack.full())
        {
            stack.push(holding);
            this->holding = nullptr;
            Logger::log(this->id, "placed a gear on " + stack.getId());
            factory->stacks_lock.unlock();
            return;
        }
    }

    factory->stacks_lock.unlock();

    // sleep before returning and trying another attempt
    // TODO - zmienna warunkowa
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void RobotA::start()
{
    if (!this->working)
    {
        this->working = true;
        std::thread worker(&RobotA::workLoop, this);
        Logger::log(this->id, "work loop started.");
    }
}

void RobotA::stop()
{
    this->working = false;
    Logger::log(this->id, "work stopped.");
}
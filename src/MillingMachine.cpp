#include "../include/MillingMachine.h"
#include <string>

unsigned int MillingMachine::count = 0;

MillingMachine::MillingMachine()
{
    this->id = "Machine" + std::to_string(this->count);
    this->count++;
}

bool MillingMachine::finished()
{
    if (this->gear != nullptr && this->gear->state == MILLED)
    {
        return true;
    }

    return false;
}

bool MillingMachine::empty()
{
    return this->gear == nullptr;
}

void MillingMachine::startMilling(Gear* gear)
{
    if (gear != nullptr)
    {
        this->gear = gear;
        std::thread worker(&MillingMachine::mill, this);
        Logger::log(this->id, "started milling.");
    }
}

void MillingMachine::mill()
{
    gear->state = MILLING;

    std::this_thread::sleep_for(std::chrono::seconds(10));
    Logger::log(this->id, "finished milling.");

    gear->state = MILLED;
}
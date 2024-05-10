#include "../include/Furnace.h"

unsigned int Furnace::count = 0;

Furnace::Furnace()
{
    this->id = "Furnace" + std::to_string(count);
    Furnace::count++;
}

std::string Furnace::getId() {
    return this->id;
}

bool Furnace::finished() 
{
    if (this->gear != nullptr && this->gear->state == CASTED)
    {
        return true;
    }

    return false;
}

bool Furnace::empty()
{
    return this->gear == nullptr;
}

Gear* Furnace::getGear()
{
    Gear* gear = this->gear;
    this->gear = nullptr;
    return gear;
}

void Furnace::startCasting(Gear* gear)
{
    if (gear != nullptr)
    {
        this->gear = gear;
        std::thread worker(&Furnace::cast, this);
        Logger::log(this->id, "started casting.");
    }

    return;
}

// runs on a dedicated thread
void Furnace::cast()
{
    gear->state = CASTING;

    std::this_thread::sleep_for(std::chrono::seconds(10));
    Logger::log(this->id, "finished casting.");

    gear->state = CASTED;
}

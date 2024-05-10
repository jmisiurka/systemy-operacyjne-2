#pragma once
#include "Gear.h"
#include <vector>

class Order
{
    std::vector<Gear> ordered_gears;
    std::vector<Gear> packaged_gears;

public:
    Order();

    void startGear();
    void packGear();
    void send();
};
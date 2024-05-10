#pragma once

enum GearState 
{
    ORDERED,
    CASTING,
    CASTED,
    MILLING,
    MILLED,
    PACKED
};

struct Gear
{
    GearState state;
    int teeth;
    float module;
};
#pragma once

#include "Furnace.h"
#include "MillingMachine.h"
#include "Order.h"
#include "Logger.h"
#include <queue>
#include <thread>
#include <string>

class Factory;

class OrderDispatcher {
    std::queue<Order> orders;
    static const std::string id;
    bool working;

    std::vector<Furnace> &furnaces;
    std::vector<MillingMachine> &machines;
    Factory* factory;

    void loop();
    void startProduction();
    void packGear();
public:
    OrderDispatcher(std::vector<Furnace> &furnaces, std::vector<MillingMachine> &machines);

    void start();
    void stop();
    void addOrder(Order order);
};
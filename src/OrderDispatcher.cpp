#include "../include/OrderDispatcher.h"
#include "../include/Factory.h"

const std::string OrderDispatcher::id = "Dispatcher";

OrderDispatcher::OrderDispatcher(std::vector<Furnace> &furnaces, std::vector<MillingMachine> &machines): furnaces(furnaces), machines(machines)
{}

void OrderDispatcher::start()
{
    this->working = true;
    std::thread worker(&OrderDispatcher::loop, this);
}

void OrderDispatcher::stop()
{
    this->working = false;
}

void OrderDispatcher::loop()
{
    while(working)
    {
        Order currentOrder;

        if (!orders.empty())
        {
            currentOrder = orders.front();
            orders.pop();
            Logger::log(this->id, "starting to work on new order.");       
        }

        factory->furnaces_lock.lock();
        for (Furnace furnace : furnaces)
        {
            if (furnace.empty())
            {
                //furnace.startCasting();
                factory->furnaces_lock.unlock();
                return;
            }
        }

        factory->furnaces_lock.unlock();
    }
}

void OrderDispatcher::addOrder(Order order)
{
    this->orders.push(order);
    Logger::log(this->id, "new order added.");
}
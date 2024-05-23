#pragma once

#include <string>
#include <iostream>

class Logger {
public:
    static void log(std::string caller_id, std::string message)
    {
        std::cout << "[LOG] " << caller_id << ": " << message << std::endl;
    }
};
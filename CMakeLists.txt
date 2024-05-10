cmake_minimum_required(VERSION 3.10)

project(SO2)

include_directories(include)

set(SOURCES src/main.cpp
            src/SupplierRobot.cpp)
add_executable(SO2 ${SOURCES})
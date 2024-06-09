cmake_minimum_required(VERSION 3.10)

project(SO2)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

include_directories(include)

file(GLOB SOURCES src/*.cpp)


add_executable(SO2 ${SOURCES})
target_link_libraries(SO2 ${CURSES_LIBRARY})
cmake_minimum_required(VERSION 3.16)
project(Advent_Of_Code_2021_Day01)

set(CMAKE_CXX_STANDARD 20)

add_executable(Day01 ${CMAKE_CURRENT_SOURCE_DIR}/day01.cpp)
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/input.txt
        ${CMAKE_CURRENT_BINARY_DIR}/input.txt
        COPYONLY
)
cmake_minimum_required(VERSION 3.16)
project(Advent_Of_Code_2021_Day16)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../common)
include(SolutionBuilder)

build_solution_for_given_day(
    INSTALL_FILE
    DAY_NUMBER "16"
)

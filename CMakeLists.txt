cmake_minimum_required(VERSION 4.0)
project(p3_part1)

set(CMAKE_CXX_STANDARD 20)

add_executable(p3_part1 main.cpp
        Scanner.cpp
        Scanner.hpp
        utils.cpp
        utils.hpp
        )

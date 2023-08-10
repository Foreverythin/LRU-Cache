cmake_minimum_required(VERSION 3.24)
project(Single-threaded)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Single-threaded
        lru.hpp
        lru.cpp
        test.cpp)

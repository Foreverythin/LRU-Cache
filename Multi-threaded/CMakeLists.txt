cmake_minimum_required(VERSION 3.24)
project(Multi-threaded)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(Multi-threaded
        test.cpp)

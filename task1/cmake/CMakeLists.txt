cmake_minimum_required(VERSION 3.10)
project(SinSum)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_DOUBLE "Use double precision instead of float" OFF)


if(USE_DOUBLE)
    message(STATUS "Using double")
    add_definitions(-DUSE_DOUBLE)
else()
    message(STATUS "Using float")
endif()

add_executable(SinSum main.cpp)
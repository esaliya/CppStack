cmake_minimum_required(VERSION 3.3)
project(omps)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")

set(SOURCE_FILES main.cpp)
add_executable(omps ${SOURCE_FILES})
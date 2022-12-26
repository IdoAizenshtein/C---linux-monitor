cmake_minimum_required(VERSION 3.6)
project(linuxMonitor)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(linuxMonitor ${SOURCE_FILES})

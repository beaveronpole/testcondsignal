cmake_minimum_required(VERSION 3.15)
project(testconditionwait)

set(CMAKE_CXX_STANDARD 14)

add_executable(testconditionwait main.cpp)

target_link_libraries(testconditionwait pthread)
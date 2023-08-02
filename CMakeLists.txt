cmake_minimum_required(VERSION 3.18)

project(test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(test test.cpp)
add_executable(mux tca9548a.cpp)

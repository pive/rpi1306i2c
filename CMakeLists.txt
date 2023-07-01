cmake_minimum_required(VERSION 3.18)

project(test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(test test.cpp)

add_subdirectory(src)

target_link_libraries(test PUBLIC rpi1306i2c)
target_include_directories(test PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/include"
                          )


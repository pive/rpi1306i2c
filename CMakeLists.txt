cmake_minimum_required(VERSION 3.18)

project(test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# NOTE: using clang libc++ for <semaphore>
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")

# enable debug prints
add_compile_definitions(RPI1306I2C_ENABLE_DEBUG)

add_executable(test test.cpp)
add_executable(mux tca9548a.cpp)

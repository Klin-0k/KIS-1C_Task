cmake_minimum_required(VERSION 3.27)
project(1C)

set(CMAKE_CXX_STANDARD 23)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/exe)

add_executable(1C main.cpp)
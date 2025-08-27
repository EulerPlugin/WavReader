cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(WavReader)

set(CMAKE_CXX_STANDARD 20)

add_executable(WavReader src/main.cpp src/WavReader.cpp)

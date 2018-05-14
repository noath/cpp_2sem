cmake_minimum_required(VERSION 3.10)
project(GlobalAllocatorSwitch)

set(CMAKE_CXX_STANDARD 14)

add_executable(GlobalAllocatorSwitch main.cpp MemoryManager.h)
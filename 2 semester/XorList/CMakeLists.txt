cmake_minimum_required(VERSION 3.10)
project(XorList)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
set(CMAKE_CXX_FLAGS "-lm -fno-stack-limit")

add_executable(XorList main.cpp XorList.h StackAllocator.h)
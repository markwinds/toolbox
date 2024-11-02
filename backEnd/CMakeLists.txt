cmake_minimum_required(VERSION 3.21)
project(back_end)

set(CMAKE_CXX_STANDARD 14)

# 三方库
include_directories(third/httpLib)

add_executable(back_end main.cpp)

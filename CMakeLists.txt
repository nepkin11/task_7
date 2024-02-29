cmake_minimum_required(VERSION 3.27)
project(Task07)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(Task07 src/main.cpp
        include/hash_set.hpp
)

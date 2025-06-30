cmake_minimum_required(VERSION 3.31)
project(hands_on_cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexec-charset=UTF-8")

set(CMAKE_CXX_STANDARD 20)

add_executable(hands_on_cpp main.cpp
        ptr_ref_test.cpp
        danling_ptr_example.cpp
)

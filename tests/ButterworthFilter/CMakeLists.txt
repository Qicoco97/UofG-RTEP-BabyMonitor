cmake_minimum_required(VERSION 3.10)
project(ButterworthFilter)

set(CMAKE_CXX_STANDARD 11)

add_executable(butterworth_test
    main.cpp
    Butterworth.cpp
)

target_include_directories(butterworth_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

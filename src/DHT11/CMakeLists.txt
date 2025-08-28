cmake_minimum_required(VERSION 3.5)

project(temp LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED IMPORTED_TARGET libgpiodcxx)
find_package(Qt5 COMPONENTS Core REQUIRED)
add_library(dht11gpiod STATIC
    DHT11Gpio.cpp
    DHT11Gpio.h
    DHT11Worker.h
    DHT11Worker.cpp
)
target_link_libraries(dht11gpiod
    PUBLIC PkgConfig::GPIOD
)
target_include_directories(dht11gpiod
    PUBLIC ${CMAKE_SOURCE_DIR}
)

#add_executable(dht11_test
#    main.cpp
#)

# ���� libgpiodcxx��C++ binding��
target_link_libraries(dht11gpiod
  PUBLIC
    PkgConfig::GPIOD
    Qt5::Core 
)
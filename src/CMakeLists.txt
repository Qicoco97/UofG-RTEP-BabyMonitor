cmake_minimum_required(VERSION 3.5)

project(baby LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PkgConfig REQUIRED)
find_package(fastrtps REQUIRED)
find_package(fastcdr   REQUIRED)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
pkg_check_modules(LIBCAMERA REQUIRED IMPORTED_TARGET libcamera)
pkg_check_modules(GPIOD REQUIRED IMPORTED_TARGET libgpiodcxx)
# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.
include_directories(${CMAKE_SOURCE_DIR} ${LIBCAMERA_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})


find_package(Qt5 COMPONENTS Widgets Charts REQUIRED)
add_subdirectory(${CMAKE_SOURCE_DIR}/dds_pub-sub
                 ${CMAKE_BINARY_DIR}/dds_pub-sub)
add_subdirectory(${CMAKE_SOURCE_DIR}/DHT11)

add_executable(baby
    main.cpp
    mainwindow.cpp
    libcam2opencv.cpp
    motionworker.cpp
    ErrorHandler.cpp
    mainwindow.h
    SensorFactory.h
    mainwindow.ui

  )

target_include_directories(baby
    PRIVATE
      ${CMAKE_SOURCE_DIR}/DHT11
)

target_link_libraries(baby PRIVATE Qt5::Widgets Qt5::Charts
    ${OpenCV_LIBS}
    Threads::Threads
    PkgConfig::LIBCAMERA
    PkgConfig::GPIOD
    fastrtps      # library fas trtps
    fastcdr 
    alarmpublisher
    dht11gpiod )

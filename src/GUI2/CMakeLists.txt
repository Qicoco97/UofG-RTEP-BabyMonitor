cmake_minimum_required(VERSION 3.5)

project(helloworld LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PkgConfig REQUIRED)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
pkg_check_modules(LIBCAMERA REQUIRED IMPORTED_TARGET libcamera)
pkg_check_modules(GPIOD REQUIRED IMPORTED_TARGET libgpiodcxx)
# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.
include_directories(${CMAKE_SOURCE_DIR} ${LIBCAMERA_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets Charts REQUIRED)


add_executable(helloworld
    main.cpp
    mainwindow.cpp
    libcam2opencv.cpp
    motionworker.cpp 
    mainwindow.h
    mainwindow.ui
  )


target_link_libraries(helloworld PRIVATE Qt5::Widgets Qt5::Charts
    ${OpenCV_LIBS}
    Threads::Threads
    PkgConfig::LIBCAMERA
    PkgConfig::GPIOD)

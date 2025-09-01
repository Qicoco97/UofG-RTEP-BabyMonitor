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


find_package(Qt5 COMPONENTS Widgets Charts Multimedia REQUIRED)
add_subdirectory(${CMAKE_SOURCE_DIR}/communication/dds
                 ${CMAKE_BINARY_DIR}/communication/dds)
add_subdirectory(${CMAKE_SOURCE_DIR}/sensors)

add_executable(baby
    main.cpp
    ui/mainwindow.cpp
    ui/controllers/CameraController.cpp
    ui/controllers/ChartController.cpp
    ui/controllers/SensorController.cpp
    ui/controllers/AlarmController.cpp
    ui/controllers/PerformanceController.cpp
    camera/libcam2opencv.cpp
    detection/motionworker.cpp
    utils/ErrorHandler.cpp
    managers/AlarmSystem.cpp
    managers/SensorManager.cpp
    core/ServiceContainer.cpp
    core/ApplicationBootstrap.cpp
    ui/mainwindow.h
    ui/controllers/IUIController.h
    ui/controllers/CameraController.h
    ui/controllers/ChartController.h
    ui/controllers/SensorController.h
    ui/controllers/AlarmController.h
    ui/controllers/PerformanceController.h
    detection/motionworker.h
    managers/AlarmSystem.h
    managers/SensorManager.h
    interfaces/IComponent.h
    sensors/SensorFactory.h
    utils/ErrorHandler.h
    performance/PerformanceMonitor.h
    ui/mainwindow.ui
  )

target_include_directories(baby
    PRIVATE
      ${CMAKE_SOURCE_DIR}/sensors
)

target_link_libraries(baby PRIVATE Qt5::Widgets Qt5::Charts Qt5::Multimedia
    ${OpenCV_LIBS}
    Threads::Threads
    PkgConfig::LIBCAMERA
    PkgConfig::GPIOD
    fastrtps      # library fas trtps
    fastcdr
    alarmpublisher
    dht11gpiod )

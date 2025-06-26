cmake_minimum_required(VERSION 3.6)

project(DDSAlarm)

find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)
find_package(PkgConfig REQUIRED)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(LIBCAMERA REQUIRED IMPORTED_TARGET libcamera)
include_directories(${CMAKE_SOURCE_DIR} ${LIBCAMERA_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
set (CMAKE_CXX_STANDARD 17)

message(STATUS "Configuring Alarm publisher/subscriber example...")


# Create a library for the message / topic
add_custom_target(
    messages
    COMMAND fastddsgen AlarmMsg.idl
)
add_library(HellowWorldMsg STATIC AlarmMsg.cxx AlarmMsgPubSubTypes.cxx)
add_dependencies(HellowWorldMsg messages)


# Publisher
add_executable(DDSAlarmPublisher 
                libcam2opencv.cpp
                 AlarmPublisher.cpp
                )
target_link_libraries(DDSAlarmPublisher fastcdr fastrtps HellowWorldMsg cam2opencv PkgConfig::LIBCAMERA ${OpenCV_LIBS})


# Subscriber
add_executable(DDSAlarmSubscriber AlarmSubscriber.cpp)
target_link_libraries(DDSAlarmSubscriber fastcdr fastrtps HellowWorldMsg)


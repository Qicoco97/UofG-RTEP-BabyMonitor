cmake_minimum_required(VERSION 3.6)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
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

add_library(alarmpublisher SHARED
      AlarmPublisher.h
     AlarmPublisher.cpp
     AlarmSubscriber.cpp
)

target_include_directories(alarmpublisher
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}        # so AlarmPublisher.h is visible to consumers
    ${fastdds_INCLUDE_DIRS}
    ${fastcdr_INCLUDE_DIRS}
)

# Publisher

target_link_libraries(alarmpublisher PUBLIC fastrtps fastcdr HellowWorldMsg cam2opencv PkgConfig::LIBCAMERA ${OpenCV_LIBS} gpiod)



# Subscriber
add_executable(DDSAlarmSubscriber AlarmSubscriber.cpp)
target_link_libraries(DDSAlarmSubscriber fastcdr fastrtps HellowWorldMsg)


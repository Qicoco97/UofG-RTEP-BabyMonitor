cmake_minimum_required(VERSION 3.14)
project(BabyMonitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Qt5
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# 查找 OpenCV
find_package(OpenCV REQUIRED)

# 查找 libcamera2opencv（你需要先 clone 或安装）
include_directories(third_party/libcamera2opencv)
file(GLOB LIBCAM_SRC third_party/libcamera2opencv/*.cpp)

# 添加 src 目录和 FrameCallback
file(GLOB SRC src/*.cpp)
file(GLOB HDR src/*.h src/*.hpp)

add_executable(baby_monitor 
    ${SRC} 
    ${HDR}
    ${LIBCAM_SRC}
    src/MainDialog.cpp
    src/FrameCallback.cpp
)

# 包含路径
target_include_directories(baby_monitor PRIVATE 
    ${OpenCV_INCLUDE_DIRS} 
    third_party/libcamera2opencv
    src
)
target_include_directories(baby_monitor PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 链接库
target_link_libraries(baby_monitor 
    Qt5::Widgets 
    ${OpenCV_LIBS}
)

# 如果 libcamera2opencv 编译成 .so 或 .a ，这里可以 target_link_libraries(baby_monitor cam2opencv)

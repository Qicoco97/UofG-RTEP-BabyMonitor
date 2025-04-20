cmake_minimum_required(VERSION 3.10)
project(MotionDetectionTests)

# 查找必要的包
find_package(OpenCV REQUIRED)
find_package(GTest REQUIRED)

# 添加测试可执行文件
add_executable(motion_detection_test motion_detection_test.cpp)

# 链接必要的库
target_link_libraries(motion_detection_test
    ${OpenCV_LIBS}
    GTest::GTest
    GTest::Main
)

# 添加包含目录
target_include_directories(motion_detection_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${OpenCV_INCLUDE_DIRS}
)

# 添加源文件
target_sources(motion_detection_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/MotionDetection.cpp
    ${CMAKE_SOURCE_DIR}/src/CommandLine.cpp
    ${CMAKE_SOURCE_DIR}/src/VideoSource.cpp
    ${CMAKE_SOURCE_DIR}/src/RieszTransform.cpp
    ${CMAKE_SOURCE_DIR}/src/WorkerThread.cpp
) 
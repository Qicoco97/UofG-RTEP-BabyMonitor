cmake_minimum_required(VERSION 3.10)
project(MotionDetectionTests)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找必要的包
find_package(OpenCV REQUIRED)

# 手动设置 Google Test 路径
set(GTEST_ROOT "/usr/src/gtest")
set(GTEST_INCLUDE_DIRS "${GTEST_ROOT}/include")
set(GTEST_LIBRARIES "/usr/lib/libgtest.a" "/usr/lib/libgtest_main.a")

# 设置源文件目录
set(SRC_DIR ${CMAKE_SOURCE_DIR}/../src)

# 添加测试可执行文件
add_executable(motion_detection_test motion_detection_test.cpp)

# 添加编译选项
target_compile_options(motion_detection_test PRIVATE
    -Wall
    -Wextra
    -pthread
    -std=c++17
)

# 链接必要的库
target_link_libraries(motion_detection_test
    ${OpenCV_LIBS}
    ${GTEST_LIBRARIES}
    pthread
    canberra
    stdc++fs
    rt
    dl
)

# 添加包含目录
target_include_directories(motion_detection_test PRIVATE
    ${SRC_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
    /usr/include
)

# 添加源文件
target_sources(motion_detection_test PRIVATE
    ${SRC_DIR}/MotionDetection.cpp
    ${SRC_DIR}/CommandLine.cpp
    ${SRC_DIR}/VideoSource.cpp
    ${SRC_DIR}/RieszTransform.cpp
    ${SRC_DIR}/Butterworth.cpp
    ${SRC_DIR}/INIReader.cpp
    ${SRC_DIR}/ini.c
)

# 添加链接器选项
set_target_properties(motion_detection_test PROPERTIES
    LINK_FLAGS "-pthread -lrt -ldl"
) 

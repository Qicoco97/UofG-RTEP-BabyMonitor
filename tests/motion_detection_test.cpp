#include <gtest/gtest.h>
#include "MotionDetection.hpp"
#include "CommandLine.hpp"
#include <opencv2/opencv.hpp>

class MotionDetectionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试用的 CommandLine 对象
        const char* argv[] = {"test", "-c", "0"};
        CommandLine cl(3, const_cast<char**>(argv));
        detector = new MotionDetection(cl);
        
        // 初始化测试用的帧
        testFrame = cv::Mat::zeros(480, 640, CV_8UC3);
    }

    void TearDown() override {
        delete detector;
    }

    MotionDetection* detector;
    cv::Mat testFrame;
};

// 测试运动检测回调
TEST_F(MotionDetectionTest, MotionCallbackTest) {
    int callbackChanges = 0;
    double callbackRate = 0.0;
    
    detector->setMotionCallback([&](int changes, double rate) {
        callbackChanges = changes;
        callbackRate = rate;
    });
    
    // 创建一个有运动的帧
    cv::rectangle(testFrame, cv::Rect(100, 100, 50, 50), cv::Scalar(255, 255, 255), -1);
    detector->update(testFrame);
    
    // 等待几帧让检测器稳定
    for(int i = 0; i < 5; i++) {
        detector->update(testFrame);
    }
    
    // 验证回调是否被调用
    EXPECT_GT(callbackChanges, 0);
    EXPECT_GT(callbackRate, 0.0);
}

// 测试报警回调
TEST_F(MotionDetectionTest, AlarmCallbackTest) {
    bool alarmTriggered = false;
    
    detector->setAlarmCallback([&]() {
        alarmTriggered = true;
    });
    
    // 更新多帧空帧以触发报警
    for(int i = 0; i < 100; i++) {
        detector->update(cv::Mat::zeros(480, 640, CV_8UC3));
    }
    
    // 验证报警回调是否被触发
    EXPECT_TRUE(alarmTriggered);
}

// 测试状态变化回调
TEST_F(MotionDetectionTest, StateChangeCallbackTest) {
    motionDetection_st lastState = init_st;
    bool stateChanged = false;
    
    detector->setStateChangeCallback([&](motionDetection_st newState) {
        lastState = newState;
        stateChanged = true;
    });
    
    // 更新几帧以触发状态变化
    for(int i = 0; i < 5; i++) {
        detector->update(testFrame);
    }
    
    // 验证状态变化回调是否被调用
    EXPECT_TRUE(stateChanged);
    EXPECT_NE(lastState, init_st);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 
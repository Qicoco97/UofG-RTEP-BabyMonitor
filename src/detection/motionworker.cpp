// motionworker.cpp
#include "motionworker.h"
#include "../performance/PerformanceMonitor.h"

MotionWorker::MotionWorker(double minArea, int thresh)
    : thresh_(thresh), minArea_(minArea)
    , performanceTimer_(new BabyMonitor::HighPrecisionTimer())
    , currentBlurKernel_(21, 21)
    , adaptiveThresh_(thresh)
    , adaptiveMinArea_(minArea)
    , isAdaptedMode_(false)
{
    // Initialize performance monitor pointer
    perfMonitor_ = &BabyMonitor::PerformanceMonitor::getInstance();
}

MotionWorker::~MotionWorker()
{
    delete performanceTimer_;
}

void MotionWorker::processFrame(const cv::Mat &currentFrame) {
    // Start performance timing
    performanceTimer_->start();

    cv::Mat gray, blur;
    cv::cvtColor(currentFrame, gray, cv::COLOR_BGR2GRAY);

    // Use adaptive blur kernel size for performance optimization
    cv::GaussianBlur(gray, blur, currentBlurKernel_, 0);

    if (previousBlur_.empty()) {
        blur.copyTo(previousBlur_);
        emit motionDetected(false);

        // Record performance even for first frame
        double processingTime = performanceTimer_->elapsedMs();
        perfMonitor_->recordLatency("MotionWorker", "MotionDetection", processingTime);
        return;
    }

    cv::Mat delta, mask;
    cv::absdiff(previousBlur_, blur, delta);
    cv::threshold(delta, mask, adaptiveThresh_, 255, cv::THRESH_BINARY);
    cv::dilate(mask, mask, {}, cv::Point(-1,-1), 2);

    std::vector<std::vector<cv::Point>> cons;
    cv::findContours(mask, cons, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    bool detected = false;
    for (auto &c : cons) {
        if (cv::contourArea(c) >= adaptiveMinArea_) {
            detected = true;
            break;
        }
    }

    previousBlur_ = blur;

    // Record performance and check for adaptation needs
    double processingTime = performanceTimer_->elapsedMs();
    if (perfMonitor_) {
        perfMonitor_->recordLatency("MotionWorker", "MotionDetection", processingTime);

        // Check if performance adaptation is needed based on average performance
        if (perfMonitor_->shouldAdaptPerformance("MotionWorker", "MotionDetection")) {
            adaptForPerformance();
        } else if (isAdaptedMode_ && perfMonitor_->canRecoverPerformance("MotionWorker", "MotionDetection")) {
            recoverPerformance();
        }
    }

    emit motionDetected(detected);
}

void MotionWorker::adaptForPerformance() {
    if (isAdaptedMode_) return; // Already adapted

    // Reduce blur kernel size for faster processing
    currentBlurKernel_ = cv::Size(15, 15);

    // Increase threshold to reduce contour processing
    adaptiveThresh_ = thresh_ + 10;

    // Increase minimum area to reduce false positives processing
    adaptiveMinArea_ = minArea_ * 1.5;

    isAdaptedMode_ = true;

    emit performanceAlert("Motion detection adapted for performance: reduced quality for speed");
}

void MotionWorker::recoverPerformance() {
    if (!isAdaptedMode_) return; // Not in adapted mode

    // Restore original parameters
    currentBlurKernel_ = cv::Size(21, 21);
    adaptiveThresh_ = thresh_;
    adaptiveMinArea_ = minArea_;

    isAdaptedMode_ = false;

    emit performanceAlert("Motion detection recovered: restored full quality");
}

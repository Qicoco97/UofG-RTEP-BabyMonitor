// motionworker.cpp
#include "motionworker.h"

MotionWorker::MotionWorker(double minArea, int thresh)
  : thresh_(thresh), minArea_(minArea) {}

void MotionWorker::processFrame(const cv::Mat &currentFrame) {
    cv::Mat gray, blur;
    cv::cvtColor(currentFrame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, blur, cv::Size(21,21), 0);

    if (previousBlur_.empty()) {
        blur.copyTo(previousBlur_);
        emit motionDetected(false);
        return;
    }

    cv::Mat delta, mask;
    cv::absdiff(previousBlur_, blur, delta);
    cv::threshold(delta, mask, thresh_, 255, cv::THRESH_BINARY);
    cv::dilate(mask, mask, {}, cv::Point(-1,-1), 2);

    std::vector<std::vector<cv::Point>> cons;
    cv::findContours(mask, cons, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    bool detected = false;
    for (auto &c : cons) {
        if (cv::contourArea(c) >= minArea_) {
            detected = true;
            break;
        }
    }

    previousBlur_ = blur;
    emit motionDetected(detected);
}

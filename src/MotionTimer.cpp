//
// Created by Congli Zhang on 2025/6/18.
//
// MotionTimer.cpp
#include "MotionTimer.h"

MotionTimer::MotionTimer(int threshold,
                         int timeoutMs,
                         int intervalMs,
                         int upsampleLevels)
        : _threshold(threshold)
        , _timeout(timeoutMs)
        , _inMotion(true)
        , _upsampleLevels(upsampleLevels)
{
    _lastMotionTime = std::chrono::steady_clock::now();
    // Start a periodic timer
    startms(intervalMs, PERIODIC);
}

MotionTimer::~MotionTimer() {
    stop();
}

void MotionTimer::setFrame(const cv::Mat& frame) {
    std::lock_guard<std::mutex> lk(_mtx);

    // Convert to grayscale
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // Process of pyrUp
    cv::Mat up = gray;
    for (int i = 0; i < _upsampleLevels; ++i) {
        cv::pyrUp(up, up);
    }

    if (!_prevGray.empty()) {
        // Difference detection
        cv::Mat diff;
        cv::absdiff(up, _prevGray, diff);
        double avg = cv::mean(diff)[0];
        auto now = std::chrono::steady_clock::now();

        if (avg > _threshold) {
            // Motion detected and time refresh
            _lastMotionTime = now;
            if (!_inMotion && _cbMotion) {
                _inMotion = true;
                _cbMotion();
            }
        }
    } else {
        _lastMotionTime = std::chrono::steady_clock::now();
    }

    // Save the current grayscale image after sampling
    _prevGray = std::move(up);
}

void MotionTimer::onNoMotion(Callback cb) {
    _cbNoMotion = std::move(cb); /// Avoiding unnecessary allocation and copying overhead
}

void MotionTimer::onMotion(Callback cb) {
    _cbMotion = std::move(cb);
}

void MotionTimer::timerEvent() {
    std::lock_guard<std::mutex> lk(_mtx);

    auto now     = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastMotionTime).count();

    if (_inMotion && elapsed > _timeout) {
        _inMotion = false;
        if (_cbNoMotion) _cbNoMotion();  /// For later process
    }
}

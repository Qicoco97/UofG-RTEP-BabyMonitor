// motionworker.h
#pragma once
#include <QObject>
#include <opencv2/opencv.hpp>
#include <memory>

// Forward declarations to avoid circular includes
namespace BabyMonitor {
    class PerformanceMonitor;
    class HighPrecisionTimer;
}

class MotionWorker : public QObject {
    Q_OBJECT
public:
    MotionWorker(double minArea = 500, int thresh = 25);
public slots:
    void processFrame(const cv::Mat &frame);
signals:
    void motionDetected(bool detected);
    void performanceAlert(const QString& message); // New signal for performance issues
private:
    cv::Mat previousBlur_;
    int thresh_;
    double minArea_;

    // Performance monitoring (using pointers to avoid include issues)
    std::unique_ptr<BabyMonitor::HighPrecisionTimer> performanceTimer_;
    BabyMonitor::PerformanceMonitor* perfMonitor_;

    // Adaptive quality parameters
    cv::Size currentBlurKernel_;
    int adaptiveThresh_;
    double adaptiveMinArea_;
    bool isAdaptedMode_;

    // Performance adaptation methods
    void adaptForPerformance();
    void recoverPerformance();
    void updateAdaptiveParameters();
};

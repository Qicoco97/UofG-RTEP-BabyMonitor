// motionworker.h
#pragma once
#include <QObject>
#include <opencv2/opencv.hpp>

class MotionWorker : public QObject {
    Q_OBJECT
public:
    MotionWorker(double minArea = 500, int thresh = 25);
public slots:
    void processFrame(const cv::Mat &frame);
signals:
    void motionDetected(bool detected);
private:
    cv::Mat previousBlur_;
    int thresh_;
    double minArea_;
};

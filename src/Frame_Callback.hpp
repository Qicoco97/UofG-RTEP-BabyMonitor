#pragma once

#include <QObject>
#include <QImage>
#include <opencv2/opencv.hpp>
#include "libcam2opencv.h"

// 回调类继承 libcamera2opencv 的 Callback 接口 + QObject（用于信号）
class FrameCallback : public QObject, public Libcam2OpenCV::Callback {
    Q_OBJECT
public:
    explicit FrameCallback(QObject *parent = nullptr) : QObject(parent) {}

    // 重写回调接口，每次有新帧时触发
    void hasFrame(const cv::Mat &frame, const libcamera::ControlList &metadata) override;

signals:
    // 自定义 Qt 信号，将 QImage 传回 GUI 主线程
    void frameReady(const QImage &image);
};

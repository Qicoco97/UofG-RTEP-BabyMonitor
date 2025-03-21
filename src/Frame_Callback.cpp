#include "FrameCallback.h"
#include <QImage>
#include <QDebug>

// libcamera2opencv 推送新帧时，会自动调用这里
void FrameCallback::hasFrame(const cv::Mat &frame, const libcamera::ControlList &)
{
    if (frame.empty()) {
        qWarning() << "Empty frame received!";
        return;
    }

    // 将 cv::Mat 转为 QImage（假设 BGR888 格式）
    QImage image(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);

    // 发射信号，主线程接收后更新 UI
    emit frameReady(image.copy());  // .copy() 保证线程安全
}

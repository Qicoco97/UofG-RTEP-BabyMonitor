#include <QCoreApplication>
#include <QImage>
#include <QDebug>
#include "FrameCallback.h"

// 模拟的数据源，模拟产生 cv::Mat 或 QImage 数据，直接触发回调
class FakeCameraSimulator {
public:
    FrameCallback* callback;

    void registerCallback(FrameCallback* cb) {
        callback = cb;
    }

    void generateFakeFrames(int count) {
        for (int i = 0; i < count; ++i) {
            // 模拟一帧灰度图（100x100），全白
            QImage img(100, 100, QImage::Format_RGB888);
            img.fill(Qt::white);

            // 模拟 cv::Mat 数据（这里只是演示，不转真的 cv::Mat）
            qDebug() << "[Simulator] Generating frame" << i;
            if (callback) {
                // 手动模拟发射信号（这里不是真libcamera帧）
                emit callback->frameReady(img);
            }
        }
    }
};

class TestReceiver : public QObject {
    Q_OBJECT
public slots:
    void onFrameReceived(const QImage& img) {
        qDebug() << "[Receiver] Frame received. Size:" << img.size();
    }
};

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // 创建回调对象
    FrameCallback callback;
    TestReceiver receiver;

    // 连接回调信号到接收槽
    QObject::connect(&callback, &FrameCallback::frameReady, &receiver, &TestReceiver::onFrameReceived);

    // 模拟摄像头，制造帧数据
    FakeCameraSimulator simulator;
    simulator.registerCallback(&callback);
    simulator.generateFakeFrames(5);  // 生成5帧测试

    return 0;
}

#include "CallbackTest.moc"

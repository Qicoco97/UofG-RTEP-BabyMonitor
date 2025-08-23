// DHT11Worker.cpp
#include "DHT11Worker.h"
#include <QThread>

void DHT11Worker::start() {
    if (running_) return;
    running_ = true;
    // �������̵߳��� run()
    QMetaObject::invokeMethod(this, "run", Qt::QueuedConnection);
}

void DHT11Worker::stop() {
    running_ = false;
}

void DHT11Worker::run() {
    // ��ʼ������ 1s �ϵ�ȴ���
    if (!sensor_.begin()) {
        emit errorReading();
        return;
    }
    while (running_) {
        if (sensor_.read()) {
            int t_int = sensor_.temperatureInt();
            int t_dec = sensor_.temperatureDec();
            int h_int = sensor_.humidityInt();
            int h_dec = sensor_.humidityDec();
            emit newReading(t_int, t_dec, h_int, h_dec);
        } else {
            emit errorReading();
        }
        // ÿ�ζ��������1�루��ͼ����
        for (int i = 0; i < 10 && running_; ++i) {
            QThread::msleep(100);
        }
    }
}

#pragma once
#include <QObject>
#include <thread>
#include <atomic>
#include "DHT11Gpio.h"

class DHT11Worker : public QObject {
    Q_OBJECT

public:
    explicit DHT11Worker(const QString &chip, unsigned int line, QObject *parent = nullptr)
      : QObject(parent)
      , sensor_(chip.toStdString(), line)
    {}

    ~DHT11Worker() {
        stop();
    }

    /// �����ڲ��߳������Զ�ȡ
    void start() {
        if (running_) return;
        running_ = true;
        workerThread_ = std::thread([this]{
            // �ϵ���ʱ + ���
            if (!sensor_.begin()) {
                emit errorReading();
                return;
            }
            while (running_) {
                if (sensor_.read()) {
                    emit newReading(
                        sensor_.temperatureInt(),
                        sensor_.temperatureDec(),
                        sensor_.humidityInt(),
                        sensor_.humidityDec()
                    );
                } else {
                    emit errorReading();
                }
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        });
    }

    /// ֹͣ�̲߳��ȴ��˳�
    void stop() {
        running_ = false;
        if (workerThread_.joinable())
            workerThread_.join();
    }

signals:
    void newReading(int t_int, int t_dec, int h_int, int h_dec);
    void errorReading();

private:
    DHT11Gpio            sensor_;
    std::thread          workerThread_;
    std::atomic<bool>    running_{false};
};

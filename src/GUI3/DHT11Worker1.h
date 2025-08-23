// DHT11Worker.h
#pragma once

#include <QObject>
#include "DHT11Gpio.h"

class DHT11Worker : public QObject {
    Q_OBJECT
public:
    explicit DHT11Worker(const QString &chip, unsigned int line, QObject *parent = nullptr)
      : QObject(parent)
      , sensor_(chip.toStdString(), line)
    {}

signals:
    void newReading(int t_int, int t_dec, int h_int, int h_dec);
    void errorReading();

public slots:
    void start();    // 启动循环
    void stop();     // 停止循环

private:
    void run();      // 循环体

    DHT11Gpio   sensor_;
    std::atomic<bool> running_{false};
};

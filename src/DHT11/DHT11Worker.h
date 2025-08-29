#pragma once
#include <QObject>
#include <thread>
#include <atomic>
#include <chrono>
#include "DHT11Gpio.h"
#include "../performance/PerformanceMonitor.h"

class DHT11Worker : public QObject {
    Q_OBJECT

public:
    explicit DHT11Worker(const QString &chip, unsigned int line, QObject *parent = nullptr, int readIntervalSeconds = 3)
      : QObject(parent)
      , sensor_(chip.toStdString(), line)
      , readInterval_(readIntervalSeconds)
      , perfMonitor_(BabyMonitor::PerformanceMonitor::getInstance())
    {}

    ~DHT11Worker() {
        stop();
    }

    /// Start internal thread for automatic reading
    void start() {
        if (running_) return;
        running_ = true;
        workerThread_ = std::thread([this]{
            // Initialize sensor + error handling
            if (!sensor_.begin()) {
                emit errorReading();
                return;
            }
            while (running_) {
                // Start sensor read timing
                BabyMonitor::HighPrecisionTimer readTimer;
                readTimer.start();

                if (sensor_.read()) {
                    // Record successful read performance
                    double readTime = readTimer.elapsedMs();
                    perfMonitor_.recordLatency("DHT11Worker", "SensorReading", readTime);

                    emit newReading(
                        sensor_.temperatureInt(),
                        sensor_.temperatureDec(),
                        sensor_.humidityInt(),
                        sensor_.humidityDec()
                    );
                } else {
                    // Record failed read performance (still useful for monitoring)
                    double readTime = readTimer.elapsedMs();
                    perfMonitor_.recordLatency("DHT11Worker", "SensorReading", readTime);

                    emit errorReading();
                }
                std::this_thread::sleep_for(std::chrono::seconds(readInterval_));
            }
        });
    }

    /// Stop thread and wait for exit
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
    int                  readInterval_;  // Read interval in seconds

    // Performance monitoring
    BabyMonitor::PerformanceMonitor& perfMonitor_;
};


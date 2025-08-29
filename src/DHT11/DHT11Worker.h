#pragma once
#include <QObject>
#include <thread>
#include <atomic>
#include <chrono>
#include "DHT11Gpio.h"

// Forward declarations
namespace BabyMonitor {
    class PerformanceMonitor;
    class HighPrecisionTimer;
}

class DHT11Worker : public QObject {
    Q_OBJECT

public:
    explicit DHT11Worker(const QString &chip, unsigned int line, QObject *parent = nullptr, int readIntervalSeconds = 3)
      : QObject(parent)
      , sensor_(chip.toStdString(), line)
      , readInterval_(readIntervalSeconds)
      , perfMonitor_(nullptr)
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
                auto startTime = std::chrono::high_resolution_clock::now();

                if (sensor_.read()) {
                    // Calculate read time and emit reading
                    auto endTime = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
                    double readTimeMs = duration.count() / 1000.0;

                    // Simple performance logging (avoid complex monitoring in thread)
                    if (readTimeMs > 500.0) { // 500ms threshold
                        // Log performance issue but don't use complex monitoring here
                    }

                    emit newReading(
                        sensor_.temperatureInt(),
                        sensor_.temperatureDec(),
                        sensor_.humidityInt(),
                        sensor_.humidityDec()
                    );
                } else {
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

    // Performance monitoring (simplified for thread safety)
    BabyMonitor::PerformanceMonitor* perfMonitor_;
};


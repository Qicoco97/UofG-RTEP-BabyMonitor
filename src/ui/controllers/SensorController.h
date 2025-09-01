// SensorController.h - Sensor management controller
#pragma once

#include "IUIController.h"
#include "../../sensors/DHT11Worker.h"
#include "../../detection/motionworker.h"
#include "../../sensors/SensorFactory.h"
#include "../../utils/SensorData.h"
#include "../../utils/ErrorHandler.h"
#include <QThread>
#include <QLabel>
#include <opencv2/opencv.hpp>

namespace BabyMonitor {

/**
 * Sensor controller - handles sensor coordination and data management
 * Single responsibility: Sensor lifecycle and data coordination
 */
class SensorController : public IUIController {
    Q_OBJECT

public:
    explicit SensorController(QLabel* tempLabel, QLabel* humLabel, QLabel* motionLabel,
                            QObject* parent = nullptr);
    ~SensorController();

    // IComponent interface
    bool initialize() override;
    void start() override;
    void stop() override;
    bool isRunning() const override { return isRunning_; }
    QString getName() const override { return "SensorController"; }
    bool isHealthy() const override;

    // IUIController interface
    void updateDisplay() override;
    void handleError(const QString& message) override;

    // Sensor-specific methods
    const TemperatureHumidityData& getLastTempHumData() const { return lastTempHumData_; }
    const MotionData& getLastMotionData() const { return lastMotionData_; }
    const SystemStatus& getSystemStatus() const { return systemStatus_; }
    bool isMotionDetected() const { return lastMotionData_.detected; }

public slots:
    void onNewDHTReading(int t_int, int t_dec, int h_int, int h_dec);
    void onDHTError();
    void onMotionStatusChanged(bool detected);
    void onMotionWorkerPerformanceAlert(const QString& message);
    void onFrameReady(const cv::Mat& frame);

signals:
    void temperatureHumidityUpdated(const TemperatureHumidityData& data);
    void motionUpdated(const MotionData& data);
    void systemStatusChanged(const SystemStatus& status);
    void sensorError(const QString& sensor, const QString& message);

private:
    // UI components
    QLabel* tempLabel_;
    QLabel* humLabel_;
    QLabel* motionLabel_;
    
    // Sensor components
    DHT11Worker* dhtWorker_;
    QThread* motionThread_;
    MotionWorker* motionWorker_;
    
    // Sensor data
    TemperatureHumidityData lastTempHumData_;
    MotionData lastMotionData_;
    SystemStatus systemStatus_;
    
    // Error tracking
    BabyMonitor::ErrorHandler& errorHandler_;
    int dht11ConsecutiveErrors_;
    static constexpr int DHT11_MAX_CONSECUTIVE_ERRORS = 5;
    
    // Helper methods
    void initializeDHT11Sensor();
    void initializeMotionDetection();
    void cleanupDHT11Sensor();
    void cleanupMotionDetection();
    void updateSensorLabels();
    void updateSystemStatus();
};

} // namespace BabyMonitor

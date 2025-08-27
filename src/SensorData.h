// SensorData.h - Structured sensor data types
#pragma once

#include <QDateTime>
#include <QString>

namespace BabyMonitor {

/**
 * Temperature and Humidity sensor data structure
 */
struct TemperatureHumidityData {
    float temperature;      // Temperature in Celsius
    float humidity;         // Humidity in percentage
    QDateTime timestamp;    // When the reading was taken
    bool isValid;          // Whether the reading is valid
    
    TemperatureHumidityData() 
        : temperature(0.0f), humidity(0.0f), timestamp(QDateTime::currentDateTime()), isValid(false) {}
    
    TemperatureHumidityData(float temp, float hum, bool valid = true)
        : temperature(temp), humidity(hum), timestamp(QDateTime::currentDateTime()), isValid(valid) {}
    
    QString toString() const {
        return QString("Temp: %1°C, Humidity: %2%, Time: %3, Valid: %4")
               .arg(temperature, 0, 'f', 1)
               .arg(humidity, 0, 'f', 1)
               .arg(timestamp.toString("hh:mm:ss"))
               .arg(isValid ? "Yes" : "No");
    }
};

/**
 * Motion detection data structure
 */
struct MotionData {
    bool detected;          // Whether motion was detected
    double confidence;      // Confidence level (0.0 - 1.0)
    QDateTime timestamp;    // When the detection occurred
    
    MotionData() 
        : detected(false), confidence(0.0), timestamp(QDateTime::currentDateTime()) {}
    
    MotionData(bool motion, double conf = 0.8)
        : detected(motion), confidence(conf), timestamp(QDateTime::currentDateTime()) {}
    
    QString toString() const {
        return QString("Motion: %1, Confidence: %2%, Time: %3")
               .arg(detected ? "Detected" : "None")
               .arg(confidence * 100, 0, 'f', 1)
               .arg(timestamp.toString("hh:mm:ss"));
    }
};

/**
 * System status data structure
 */
struct SystemStatus {
    bool cameraActive;
    bool dht11Active;
    bool motionDetectionActive;
    bool alarmSystemActive;
    QDateTime lastUpdate;
    
    SystemStatus()
        : cameraActive(false), dht11Active(false), 
          motionDetectionActive(false), alarmSystemActive(false),
          lastUpdate(QDateTime::currentDateTime()) {}
    
    bool isAllSystemsActive() const {
        return cameraActive && dht11Active && motionDetectionActive && alarmSystemActive;
    }
    
    QString toString() const {
        return QString("Camera: %1, DHT11: %2, Motion: %3, Alarm: %4")
               .arg(cameraActive ? "OK" : "OFF")
               .arg(dht11Active ? "OK" : "OFF")
               .arg(motionDetectionActive ? "OK" : "OFF")
               .arg(alarmSystemActive ? "OK" : "OFF");
    }
};

} // namespace BabyMonitor

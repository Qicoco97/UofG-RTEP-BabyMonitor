// SensorData.h - Structured sensor data types with proper encapsulation
#pragma once

#include <QDateTime>
#include <QString>

namespace BabyMonitor {

/**
 * Temperature and Humidity sensor data class with proper encapsulation
 * Provides data validation and controlled access to sensor readings
 */
class TemperatureHumidityData {
private:
    float temperature_;      // Temperature in Celsius
    float humidity_;         // Humidity in percentage
    QDateTime timestamp_;    // When the reading was taken
    bool isValid_;          // Whether the reading is valid

public:
    // Constructors
    TemperatureHumidityData()
        : temperature_(0.0f), humidity_(0.0f), timestamp_(QDateTime::currentDateTime()), isValid_(false) {}

    TemperatureHumidityData(float temp, float hum, bool valid = true)
        : temperature_(temp), humidity_(hum), timestamp_(QDateTime::currentDateTime()), isValid_(valid) {
        validateData();
    }

    // Getters (const methods for read-only access)
    float getTemperature() const { return temperature_; }
    float getHumidity() const { return humidity_; }
    QDateTime getTimestamp() const { return timestamp_; }
    bool getIsValid() const { return isValid_; }

    // Setters with validation
    void setTemperature(float temp) {
        temperature_ = temp;
        validateData();
    }

    void setHumidity(float hum) {
        humidity_ = hum;
        validateData();
    }

    void setValid(bool valid) {
        isValid_ = valid;
    }

    void updateTimestamp() {
        timestamp_ = QDateTime::currentDateTime();
    }

    // Public methods
    QString toString() const {
        return QString("Temp: %1°C, Humidity: %2%, Time: %3, Valid: %4")
               .arg(temperature_, 0, 'f', 1)
               .arg(humidity_, 0, 'f', 1)
               .arg(timestamp_.toString("hh:mm:ss"))
               .arg(isValid_ ? "Yes" : "No");
    }

    // Backward compatibility - public member access (deprecated but functional)
    // These allow existing code to continue working while we transition to getters/setters
    float& temperature = temperature_;
    float& humidity = humidity_;
    QDateTime& timestamp = timestamp_;
    bool& isValid = isValid_;

private:
    void validateData() {
        // Validate temperature range (-40°C to 80°C is reasonable for DHT11)
        if (temperature_ < -40.0f || temperature_ > 80.0f) {
            isValid_ = false;
            return;
        }

        // Validate humidity range (0% to 100%)
        if (humidity_ < 0.0f || humidity_ > 100.0f) {
            isValid_ = false;
            return;
        }

        // If we reach here, data is valid
        if (isValid_ != false) { // Don't override if explicitly set to false
            isValid_ = true;
        }
    }
};

/**
 * Motion detection data class with proper encapsulation
 */
class MotionData {
private:
    bool detected_;          // Whether motion was detected
    double confidence_;      // Confidence level (0.0 - 1.0)
    QDateTime timestamp_;    // When the detection occurred

public:
    // Constructors
    MotionData()
        : detected_(false), confidence_(0.0), timestamp_(QDateTime::currentDateTime()) {}

    MotionData(bool motion, double conf = 0.8)
        : detected_(motion), confidence_(conf), timestamp_(QDateTime::currentDateTime()) {
        validateData();
    }

    // Getters
    bool getDetected() const { return detected_; }
    double getConfidence() const { return confidence_; }
    QDateTime getTimestamp() const { return timestamp_; }

    // Setters with validation
    void setDetected(bool motion) {
        detected_ = motion;
    }

    void setConfidence(double conf) {
        confidence_ = conf;
        validateData();
    }

    void updateTimestamp() {
        timestamp_ = QDateTime::currentDateTime();
    }

    // Public methods
    QString toString() const {
        return QString("Motion: %1, Confidence: %2%, Time: %3")
               .arg(detected_ ? "Detected" : "None")
               .arg(confidence_ * 100, 0, 'f', 1)
               .arg(timestamp_.toString("hh:mm:ss"));
    }

    // Backward compatibility - public member access (deprecated but functional)
    bool& detected = detected_;
    double& confidence = confidence_;
    QDateTime& timestamp = timestamp_;

private:
    void validateData() {
        // Validate confidence range (0.0 to 1.0)
        if (confidence_ < 0.0) {
            confidence_ = 0.0;
        } else if (confidence_ > 1.0) {
            confidence_ = 1.0;
        }
    }
};

/**
 * System status data class with proper encapsulation
 */
class SystemStatus {
private:
    bool cameraActive_;
    bool dht11Active_;
    bool motionDetectionActive_;
    bool alarmSystemActive_;
    QDateTime lastUpdate_;

public:
    // Constructor
    SystemStatus()
        : cameraActive_(false), dht11Active_(false),
          motionDetectionActive_(false), alarmSystemActive_(false),
          lastUpdate_(QDateTime::currentDateTime()) {}

    // Getters
    bool getCameraActive() const { return cameraActive_; }
    bool getDht11Active() const { return dht11Active_; }
    bool getMotionDetectionActive() const { return motionDetectionActive_; }
    bool getAlarmSystemActive() const { return alarmSystemActive_; }
    QDateTime getLastUpdate() const { return lastUpdate_; }

    // Setters
    void setCameraActive(bool active) {
        cameraActive_ = active;
        updateTimestamp();
    }

    void setDht11Active(bool active) {
        dht11Active_ = active;
        updateTimestamp();
    }

    void setMotionDetectionActive(bool active) {
        motionDetectionActive_ = active;
        updateTimestamp();
    }

    void setAlarmSystemActive(bool active) {
        alarmSystemActive_ = active;
        updateTimestamp();
    }

    void updateTimestamp() {
        lastUpdate_ = QDateTime::currentDateTime();
    }

    // Public methods
    bool isAllSystemsActive() const {
        return cameraActive_ && dht11Active_ && motionDetectionActive_ && alarmSystemActive_;
    }

    QString toString() const {
        return QString("Camera: %1, DHT11: %2, Motion: %3, Alarm: %4")
               .arg(cameraActive_ ? "OK" : "OFF")
               .arg(dht11Active_ ? "OK" : "OFF")
               .arg(motionDetectionActive_ ? "OK" : "OFF")
               .arg(alarmSystemActive_ ? "OK" : "OFF");
    }

    // Backward compatibility - public member access (deprecated but functional)
    bool& cameraActive = cameraActive_;
    bool& dht11Active = dht11Active_;
    bool& motionDetectionActive = motionDetectionActive_;
    bool& alarmSystemActive = alarmSystemActive_;
    QDateTime& lastUpdate = lastUpdate_;
};

} // namespace BabyMonitor

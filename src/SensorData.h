// SensorData.h - Structured sensor data types
#pragma once

#include <QDateTime>
#include <QString>

namespace BabyMonitor {

/**
 * Temperature and Humidity sensor data class (improved from struct)
 * Provides better encapsulation while maintaining backward compatibility
 */
class TemperatureHumidityData {
public:
    // Public member variables for backward compatibility
    float temperature;      // Temperature in Celsius
    float humidity;         // Humidity in percentage
    QDateTime timestamp;    // When the reading was taken
    bool isValid;          // Whether the reading is valid

    // Constructors
    TemperatureHumidityData()
        : temperature(0.0f), humidity(0.0f), timestamp(QDateTime::currentDateTime()), isValid(false) {}

    TemperatureHumidityData(float temp, float hum, bool valid = true)
        : temperature(temp), humidity(hum), timestamp(QDateTime::currentDateTime()), isValid(valid) {
        validateData();
    }

    // Copy constructor
    TemperatureHumidityData(const TemperatureHumidityData& other)
        : temperature(other.temperature), humidity(other.humidity),
          timestamp(other.timestamp), isValid(other.isValid) {}

    // Assignment operator
    TemperatureHumidityData& operator=(const TemperatureHumidityData& other) {
        if (this != &other) {
            temperature = other.temperature;
            humidity = other.humidity;
            timestamp = other.timestamp;
            isValid = other.isValid;
        }
        return *this;
    }

    // Improved methods for better encapsulation (recommended for new code)
    float getTemperature() const { return temperature; }
    float getHumidity() const { return humidity; }
    QDateTime getTimestamp() const { return timestamp; }
    bool getIsValid() const { return isValid; }

    void setTemperature(float temp) {
        temperature = temp;
        validateData();
    }

    void setHumidity(float hum) {
        humidity = hum;
        validateData();
    }

    void setValid(bool valid) {
        isValid = valid;
    }

    void updateTimestamp() {
        timestamp = QDateTime::currentDateTime();
    }

    QString toString() const {
        return QString("Temp: %1°C, Humidity: %2%, Time: %3, Valid: %4")
               .arg(temperature, 0, 'f', 1)
               .arg(humidity, 0, 'f', 1)
               .arg(timestamp.toString("hh:mm:ss"))
               .arg(isValid ? "Yes" : "No");
    }

    // Data validation method
    void validateData() {
        // Validate temperature range (-40°C to 80°C is reasonable for DHT11)
        if (temperature < -40.0f || temperature > 80.0f) {
            isValid = false;
            return;
        }

        // Validate humidity range (0% to 100%)
        if (humidity < 0.0f || humidity > 100.0f) {
            isValid = false;
            return;
        }

        // If we reach here, data is valid
        if (isValid != false) { // Don't override if explicitly set to false
            isValid = true;
        }
    }
};

/**
 * Motion detection data class (improved from struct)
 * Provides better encapsulation while maintaining backward compatibility
 */
class MotionData {
public:
    // Public member variables for backward compatibility
    bool detected;          // Whether motion was detected
    double confidence;      // Confidence level (0.0 - 1.0)
    QDateTime timestamp;    // When the detection occurred

    // Constructors
    MotionData()
        : detected(false), confidence(0.0), timestamp(QDateTime::currentDateTime()) {}

    MotionData(bool motion, double conf = 0.8)
        : detected(motion), confidence(conf), timestamp(QDateTime::currentDateTime()) {
        validateData();
    }

    // Copy constructor
    MotionData(const MotionData& other)
        : detected(other.detected), confidence(other.confidence), timestamp(other.timestamp) {}

    // Assignment operator
    MotionData& operator=(const MotionData& other) {
        if (this != &other) {
            detected = other.detected;
            confidence = other.confidence;
            timestamp = other.timestamp;
        }
        return *this;
    }

    // Improved methods for better encapsulation (recommended for new code)
    bool getDetected() const { return detected; }
    double getConfidence() const { return confidence; }
    QDateTime getTimestamp() const { return timestamp; }

    void setDetected(bool motion) {
        detected = motion;
    }

    void setConfidence(double conf) {
        confidence = conf;
        validateData();
    }

    void updateTimestamp() {
        timestamp = QDateTime::currentDateTime();
    }

    QString toString() const {
        return QString("Motion: %1, Confidence: %2%, Time: %3")
               .arg(detected ? "Detected" : "None")
               .arg(confidence * 100, 0, 'f', 1)
               .arg(timestamp.toString("hh:mm:ss"));
    }

    // Data validation method
    void validateData() {
        // Validate confidence range (0.0 to 1.0)
        if (confidence < 0.0) {
            confidence = 0.0;
        } else if (confidence > 1.0) {
            confidence = 1.0;
        }
    }
};

/**
 * System status data class (improved from struct)
 * Provides better encapsulation while maintaining backward compatibility
 */
class SystemStatus {
public:
    // Public member variables for backward compatibility
    bool cameraActive;
    bool dht11Active;
    bool motionDetectionActive;
    bool alarmSystemActive;
    QDateTime lastUpdate;

    // Constructor
    SystemStatus()
        : cameraActive(false), dht11Active(false),
          motionDetectionActive(false), alarmSystemActive(false),
          lastUpdate(QDateTime::currentDateTime()) {}

    // Copy constructor
    SystemStatus(const SystemStatus& other)
        : cameraActive(other.cameraActive), dht11Active(other.dht11Active),
          motionDetectionActive(other.motionDetectionActive),
          alarmSystemActive(other.alarmSystemActive), lastUpdate(other.lastUpdate) {}

    // Assignment operator
    SystemStatus& operator=(const SystemStatus& other) {
        if (this != &other) {
            cameraActive = other.cameraActive;
            dht11Active = other.dht11Active;
            motionDetectionActive = other.motionDetectionActive;
            alarmSystemActive = other.alarmSystemActive;
            lastUpdate = other.lastUpdate;
        }
        return *this;
    }

    // Improved methods for better encapsulation (recommended for new code)
    bool getCameraActive() const { return cameraActive; }
    bool getDht11Active() const { return dht11Active; }
    bool getMotionDetectionActive() const { return motionDetectionActive; }
    bool getAlarmSystemActive() const { return alarmSystemActive; }
    QDateTime getLastUpdate() const { return lastUpdate; }

    void setCameraActive(bool active) {
        cameraActive = active;
        updateTimestamp();
    }

    void setDht11Active(bool active) {
        dht11Active = active;
        updateTimestamp();
    }

    void setMotionDetectionActive(bool active) {
        motionDetectionActive = active;
        updateTimestamp();
    }

    void setAlarmSystemActive(bool active) {
        alarmSystemActive = active;
        updateTimestamp();
    }

    void updateTimestamp() {
        lastUpdate = QDateTime::currentDateTime();
    }

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

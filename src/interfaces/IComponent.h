// IComponent.h - Basic component interfaces
#pragma once

#include <QString>

namespace BabyMonitor {

/**
 * Base interface for all system components
 * Follows Interface Segregation Principle
 */
class IComponent {
public:
    virtual ~IComponent() = default;
    
    virtual bool initialize() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    virtual QString getName() const = 0;
    virtual bool isHealthy() const = 0;
};

/**
 * Sensor manager interface
 * Manages multiple sensors as a group
 */
class ISensorManager : public IComponent {
public:
    virtual ~ISensorManager() = default;
    
    virtual bool addSensor(const QString& name, IComponent* sensor) = 0;
    virtual bool removeSensor(const QString& name) = 0;
    virtual IComponent* getSensor(const QString& name) const = 0;
    virtual QStringList getActiveSensors() const = 0;
    virtual QStringList getErrorSensors() const = 0;
    virtual bool areAllSensorsHealthy() const = 0;
};

/**
 * Alarm system interface
 * Handles alarm publishing and management
 */
class IAlarmSystem : public IComponent {
public:
    virtual ~IAlarmSystem() = default;
    
    virtual bool publishAlarm(const QString& message, int severity = 1) = 0;
    virtual bool hasSubscribers() const = 0;
    virtual void setPublishInterval(int intervalMs) = 0;
};

/**
 * Display manager interface
 * Handles UI updates and chart management
 */
class IDisplayManager : public IComponent {
public:
    virtual ~IDisplayManager() = default;
    
    virtual void updateTemperatureDisplay(float temperature) = 0;
    virtual void updateHumidityDisplay(float humidity) = 0;
    virtual void updateMotionDisplay(bool detected) = 0;
    virtual void updateSystemStatus(const QString& status) = 0;
    virtual void showError(const QString& message) = 0;
};

} // namespace BabyMonitor

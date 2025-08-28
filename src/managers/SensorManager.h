// SensorManager.h - Implementation of sensor management
#pragma once

#include <QObject>
#include <QMap>
#include <memory>
#include "../interfaces/IComponent.h"
#include "../ErrorHandler.h"

namespace BabyMonitor {

/**
 * Sensor manager implementation
 * Follows Single Responsibility Principle - only manages sensors
 */
class SensorManager : public QObject, public ISensorManager {
    Q_OBJECT

public:
    explicit SensorManager(QObject* parent = nullptr);
    ~SensorManager();

    // IComponent interface
    bool initialize() override;
    void start() override;
    void stop() override;
    bool isRunning() const override;
    QString getName() const override { return "SensorManager"; }
    bool isHealthy() const override;

    // ISensorManager interface
    bool addSensor(const QString& name, IComponent* sensor) override;
    bool removeSensor(const QString& name) override;
    IComponent* getSensor(const QString& name) const override;
    QStringList getActiveSensors() const override;
    QStringList getErrorSensors() const override;
    bool areAllSensorsHealthy() const override;

signals:
    void sensorAdded(const QString& name);
    void sensorRemoved(const QString& name);
    void sensorStatusChanged(const QString& name, bool isHealthy);
    void allSensorsHealthy();
    void sensorsWithErrors(const QStringList& errorSensors);

private:
    QMap<QString, IComponent*> sensors_;
    bool isRunning_;
    ErrorHandler& errorHandler_;
    
    void checkSensorHealth();
};

} // namespace BabyMonitor

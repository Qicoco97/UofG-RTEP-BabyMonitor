// SensorManager.cpp - Sensor manager implementation
#include "SensorManager.h"

namespace BabyMonitor {

SensorManager::SensorManager(QObject* parent)
    : QObject(parent)
    , isRunning_(false)
    , errorHandler_(ErrorHandler::getInstance())
{
}

SensorManager::~SensorManager()
{
    stop();
}

bool SensorManager::initialize()
{
    errorHandler_.reportInfo("SensorManager", "Initializing sensor manager");
    
    bool allInitialized = true;
    for (auto it = sensors_.begin(); it != sensors_.end(); ++it) {
        if (!it.value()->initialize()) {
            errorHandler_.reportError("SensorManager", 
                QString("Failed to initialize sensor: %1").arg(it.key()));
            allInitialized = false;
        }
    }
    
    if (allInitialized) {
        errorHandler_.reportInfo("SensorManager", "All sensors initialized successfully");
    }
    
    return allInitialized;
}

void SensorManager::start()
{
    if (isRunning_) return;
    
    errorHandler_.reportInfo("SensorManager", "Starting all sensors");
    
    for (auto it = sensors_.begin(); it != sensors_.end(); ++it) {
        it.value()->start();
        errorHandler_.reportInfo("SensorManager", 
            QString("Started sensor: %1").arg(it.key()));
    }
    
    isRunning_ = true;
    checkSensorHealth();
}

void SensorManager::stop()
{
    if (!isRunning_) return;
    
    errorHandler_.reportInfo("SensorManager", "Stopping all sensors");
    
    for (auto it = sensors_.begin(); it != sensors_.end(); ++it) {
        it.value()->stop();
        errorHandler_.reportInfo("SensorManager", 
            QString("Stopped sensor: %1").arg(it.key()));
    }
    
    isRunning_ = false;
}

bool SensorManager::isRunning() const
{
    return isRunning_;
}

bool SensorManager::isHealthy() const
{
    return areAllSensorsHealthy();
}

bool SensorManager::addSensor(const QString& name, IComponent* sensor)
{
    if (sensors_.contains(name)) {
        errorHandler_.reportWarning("SensorManager", 
            QString("Sensor already exists: %1").arg(name));
        return false;
    }
    
    sensors_[name] = sensor;
    errorHandler_.reportInfo("SensorManager", 
        QString("Added sensor: %1").arg(name));
    
    emit sensorAdded(name);
    return true;
}

bool SensorManager::removeSensor(const QString& name)
{
    if (!sensors_.contains(name)) {
        errorHandler_.reportWarning("SensorManager", 
            QString("Sensor not found: %1").arg(name));
        return false;
    }
    
    IComponent* sensor = sensors_.take(name);
    if (sensor && sensor->isRunning()) {
        sensor->stop();
    }
    
    errorHandler_.reportInfo("SensorManager", 
        QString("Removed sensor: %1").arg(name));
    
    emit sensorRemoved(name);
    return true;
}

IComponent* SensorManager::getSensor(const QString& name) const
{
    return sensors_.value(name, nullptr);
}

QStringList SensorManager::getActiveSensors() const
{
    QStringList active;
    for (auto it = sensors_.begin(); it != sensors_.end(); ++it) {
        if (it.value()->isRunning()) {
            active << it.key();
        }
    }
    return active;
}

QStringList SensorManager::getErrorSensors() const
{
    QStringList errors;
    for (auto it = sensors_.begin(); it != sensors_.end(); ++it) {
        if (!it.value()->isHealthy()) {
            errors << it.key();
        }
    }
    return errors;
}

bool SensorManager::areAllSensorsHealthy() const
{
    for (auto it = sensors_.begin(); it != sensors_.end(); ++it) {
        if (!it.value()->isHealthy()) {
            return false;
        }
    }
    return true;
}

void SensorManager::checkSensorHealth()
{
    QStringList errorSensors = getErrorSensors();
    
    if (errorSensors.isEmpty()) {
        emit allSensorsHealthy();
    } else {
        emit sensorsWithErrors(errorSensors);
    }
}

} // namespace BabyMonitor

#include "SensorManager.moc"

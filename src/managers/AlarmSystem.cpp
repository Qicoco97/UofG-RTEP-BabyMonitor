// AlarmSystem.cpp - Alarm system implementation
#include "AlarmSystem.h"
#include "../dds_pub-sub/AlarmMsg.h"
#include <QDateTime>

namespace BabyMonitor {

AlarmSystem::AlarmSystem(QObject* parent)
    : QObject(parent)
    , isInitialized_(false)
    , isRunning_(false)
    , publishInterval_(1000)
    , errorHandler_(ErrorHandler::getInstance())
{
}

AlarmSystem::~AlarmSystem()
{
    stop();
}

bool AlarmSystem::initialize()
{
    if (isInitialized_) return true;
    
    errorHandler_.reportInfo("AlarmSystem", "Initializing alarm system");
    
    if (alarmPublisher_.init()) {
        isInitialized_ = true;
        errorHandler_.reportInfo("AlarmSystem", "Initialized successfully");
        return true;
    } else {
        errorHandler_.reportError("AlarmSystem", "Initialization failed");
        return false;
    }
}

void AlarmSystem::start()
{
    if (!isInitialized_) {
        errorHandler_.reportError("AlarmSystem", "Cannot start - not initialized");
        return;
    }
    
    if (isRunning_) return;
    
    isRunning_ = true;
    errorHandler_.reportInfo("AlarmSystem", "Alarm system started");
}

void AlarmSystem::stop()
{
    if (!isRunning_) return;
    
    isRunning_ = false;
    errorHandler_.reportInfo("AlarmSystem", "Alarm system stopped");
}

bool AlarmSystem::isRunning() const
{
    return isRunning_;
}

bool AlarmSystem::isHealthy() const
{
    return isInitialized_ && isRunning_;
}

bool AlarmSystem::publishAlarm(const QString& message, int severity)
{
    if (!isRunning_) {
        errorHandler_.reportWarning("AlarmSystem", "Cannot publish - system not running");
        return false;
    }
    
    QString formattedMessage = formatAlarmMessage(message, severity);

    // Create AlarmMsg and publish using the correct method
    AlarmMsg msg;
    msg.index(severity);
    msg.message(formattedMessage.toStdString());

    if (alarmPublisher_.publish(msg)) {
        errorHandler_.reportInfo("AlarmSystem", QString("Alarm published: %1").arg(message));
        emit alarmPublished(message);
        return true;
    } else {
        errorHandler_.reportError("AlarmSystem", QString("Failed to publish alarm: %1").arg(message));
        emit publishFailed("DDS publish failed");
        return false;
    }
}

bool AlarmSystem::hasSubscribers() const
{
    // AlarmPublisher doesn't provide subscriber count, assume false for now
    return false;
}

void AlarmSystem::setPublishInterval(int intervalMs)
{
    publishInterval_ = intervalMs;
    errorHandler_.reportInfo("AlarmSystem", 
        QString("Publish interval set to %1ms").arg(intervalMs));
}

QString AlarmSystem::formatAlarmMessage(const QString& message, int severity) const
{
    QDateTime now = QDateTime::currentDateTime();
    return QString("[%1] Severity:%2 - %3")
           .arg(now.toString("yyyy-MM-dd hh:mm:ss"))
           .arg(severity)
           .arg(message);
}

} // namespace BabyMonitor

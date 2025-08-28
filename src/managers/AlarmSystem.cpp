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
        // Successfully published to subscribers
        errorHandler_.reportInfo("AlarmSystem", QString("Alarm published: %1").arg(message));
        emit alarmPublished(message);
        return true;
    } else {
        // No subscribers available - this is normal, not an error
        // Only log at debug level to avoid spam
        static int noListenerCount = 0;
        if (++noListenerCount % 10 == 0) {  // Report every 10th occurrence
            errorHandler_.reportInfo("AlarmSystem",
                QString("No subscribers for alarm (count: %1)").arg(noListenerCount));
        }
        return true;  // Return true since this is not actually a failure
    }
}

bool AlarmSystem::hasSubscribers() const
{
    // AlarmPublisher doesn't provide subscriber count, assume false for now
    return false;
}

void AlarmSystem::setPublishInterval(int intervalMs)
{
    // Data validation
    if (intervalMs < 100) {
        errorHandler_.reportWarning("AlarmSystem",
            QString("Interval %1ms too small, using minimum 100ms").arg(intervalMs));
        intervalMs = 100;
    }
    if (intervalMs > 60000) {
        errorHandler_.reportWarning("AlarmSystem",
            QString("Interval %1ms too large, using maximum 60000ms").arg(intervalMs));
        intervalMs = 60000;
    }

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

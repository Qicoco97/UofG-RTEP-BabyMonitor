// AlarmSystem.h - Alarm system implementation
#pragma once

#include <QObject>
#include <QTimer>
#include <memory>
#include "../interfaces/IComponent.h"
#include "../dds_pub-sub/AlarmPublisher.h"
#include "../ErrorHandler.h"

// Forward declarations
namespace BabyMonitor {
    class PerformanceMonitor;
    class HighPrecisionTimer;
}

namespace BabyMonitor {

/**
 * Alarm system implementation
 * Wraps the existing AlarmPublisher with interface compliance
 */
class AlarmSystem : public QObject, public IAlarmSystem {
    Q_OBJECT

public:
    explicit AlarmSystem(QObject* parent = nullptr);
    ~AlarmSystem();

    // IComponent interface
    bool initialize() override;
    void start() override;
    void stop() override;
    bool isRunning() const override;
    QString getName() const override { return "AlarmSystem"; }
    bool isHealthy() const override;

    // IAlarmSystem interface
    bool publishAlarm(const QString& message, int severity = 1) override;
    bool hasSubscribers() const override;
    void setPublishInterval(int intervalMs) override;

signals:
    void alarmPublished(const QString& message);
    void publishFailed(const QString& error);

private:
    AlarmPublisher alarmPublisher_;
    bool isInitialized_;
    bool isRunning_;
    int publishInterval_;
    ErrorHandler& errorHandler_;

    // Performance monitoring (using pointers to avoid include issues)
    BabyMonitor::PerformanceMonitor* perfMonitor_;
    std::unique_ptr<BabyMonitor::HighPrecisionTimer> publishTimer_;
    bool isAdaptedPublishMode_;
    int adaptivePublishInterval_;

    QString formatAlarmMessage(const QString& message, int severity) const;
    void adaptPublishFrequency();
    void recoverPublishFrequency();
};

} // namespace BabyMonitor

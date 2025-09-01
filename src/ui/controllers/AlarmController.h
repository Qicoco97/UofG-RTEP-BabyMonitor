// AlarmController.h - Alarm system controller
#pragma once

#include "IUIController.h"
#include "../../hardware/LedController.h"
#include "../../managers/AlarmSystem.h"
#include "../../utils/SensorData.h"
#include "../../utils/Config.h"
#include "../../utils/ErrorHandler.h"
#include "../../performance/PerformanceMonitor.h"
#include <QTimer>
#include <QMediaPlayer>
#include <QUrl>
#include <memory>

namespace BabyMonitor {

/**
 * Alarm controller - handles alarm logic and hardware control
 * Single responsibility: Alarm system coordination and hardware control
 */
class AlarmController : public IUIController {
    Q_OBJECT

public:
    explicit AlarmController(QObject* parent = nullptr);
    ~AlarmController();

    // IComponent interface
    bool initialize() override;
    void start() override;
    void stop() override;
    bool isRunning() const override { return isRunning_; }
    QString getName() const override { return "AlarmController"; }
    bool isHealthy() const override;

    // IUIController interface
    void updateDisplay() override;
    void handleError(const QString& message) override;

    // Alarm-specific methods
    void setAlarmSystem(std::shared_ptr<IAlarmSystem> alarmSystem);
    void processMotionStatus(bool motionDetected);
    void triggerMotionAlert();
    void playAlarmSound();

public slots:
    void onMotionUpdate(const MotionData& data);
    void onAlarmTimerTimeout();
    void onAudioPlayerStateChanged(QMediaPlayer::State state);

signals:
    void alarmTriggered(const QString& message, int severity);
    void ledStatusChanged(bool active);

private:
    // Alarm system components
    std::shared_ptr<IAlarmSystem> alarmSystem_;
    LEDController led_;
    QMediaPlayer* audioPlayer_;
    
    // Alarm timing
    QTimer* alarmTimer_;
    BabyMonitor::HighPrecisionTimer* alarmResponseTimer_;
    
    // Alarm state
    uint32_t samplesSent_;
    int noMotionCount_;
    bool alarmPlaying_;
    bool motionDetected_;
    
    // Error handling and performance
    BabyMonitor::ErrorHandler& errorHandler_;
    BabyMonitor::PerformanceMonitor* perfMonitor_;
    
    // Helper methods
    void initializeLED();
    void initializeAudioPlayer();
    void initializeAlarmTimer();
    void publishAlarmMessage(const QString& message, int severity);
    void handleNoMotionAlarm();
    void handleMotionDetectedAlarm();
};

} // namespace BabyMonitor

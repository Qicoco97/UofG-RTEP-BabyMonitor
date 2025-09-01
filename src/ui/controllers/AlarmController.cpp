// AlarmController.cpp - Alarm system controller implementation
#include "AlarmController.h"
#include <QDebug>

// Ensure Qt macros are defined
#ifndef signals
#define signals Q_SIGNALS
#endif
#ifndef slots
#define slots Q_SLOTS
#endif
#ifndef emit
#define emit Q_EMIT
#endif

namespace BabyMonitor {

AlarmController::AlarmController(QObject* parent)
    : IUIController(parent)
    , led_(BabyMonitorConfig::LED_CHIP_NUMBER, BabyMonitorConfig::LED_PIN_NUMBER)
    , audioPlayer_(nullptr)
    , alarmTimer_(nullptr)
    , alarmResponseTimer_(new BabyMonitor::HighPrecisionTimer())
    , samplesSent_(1)
    , noMotionCount_(0)
    , alarmPlaying_(false)
    , motionDetected_(false)
    , errorHandler_(BabyMonitor::ErrorHandler::getInstance())
    , perfMonitor_(&BabyMonitor::PerformanceMonitor::getInstance())
{
}

AlarmController::~AlarmController()
{
    stop();
    delete alarmResponseTimer_;
}

bool AlarmController::initialize()
{
    if (isInitialized_) return true;
    
    errorHandler_.reportInfo("AlarmController", "Initializing alarm controller");
    
    try {
        initializeLED();
        initializeAudioPlayer();
        initializeAlarmTimer();
        
        isInitialized_ = true;
        errorHandler_.reportInfo("AlarmController", "Initialized successfully");
        return true;
    } catch (const std::exception& e) {
        errorHandler_.reportError("AlarmController", "Initialization failed", e.what());
        return false;
    }
}

void AlarmController::start()
{
    if (!isInitialized_) {
        errorHandler_.reportError("AlarmController", "Cannot start - not initialized");
        return;
    }
    
    if (isRunning_) return;
    
    try {
        if (alarmTimer_) {
            alarmTimer_->start(BabyMonitorConfig::ALARM_TIMER_INTERVAL_MS);
        }
        
        isRunning_ = true;
        errorHandler_.reportInfo("AlarmController", "Started successfully");
        emit statusChanged("Alarm: Active");
    } catch (const std::exception& e) {
        errorHandler_.reportError("AlarmController", "Failed to start", e.what());
        emit errorOccurred("AlarmController", "Failed to start");
    }
}

void AlarmController::stop()
{
    if (!isRunning_) return;
    
    try {
        if (alarmTimer_) {
            alarmTimer_->stop();
        }
        
        if (audioPlayer_ && alarmPlaying_) {
            audioPlayer_->stop();
            alarmPlaying_ = false;
        }
        
        isRunning_ = false;
        errorHandler_.reportInfo("AlarmController", "Stopped successfully");
        emit statusChanged("Alarm: Stopped");
    } catch (const std::exception& e) {
        errorHandler_.reportError("AlarmController", "Error stopping", e.what());
    }
}

bool AlarmController::isHealthy() const
{
    return isInitialized_ && isRunning_ && alarmSystem_ != nullptr;
}

void AlarmController::updateDisplay()
{
    // Alarm controller doesn't directly manage display elements
    // Status updates are sent via signals
}

void AlarmController::handleError(const QString& message)
{
    errorHandler_.reportError("AlarmController", message);
    emit errorOccurred("AlarmController", message);
}

void AlarmController::setAlarmSystem(std::shared_ptr<IAlarmSystem> alarmSystem)
{
    alarmSystem_ = alarmSystem;
    if (alarmSystem_) {
        errorHandler_.reportInfo("AlarmController", "Alarm system injected successfully");
    }
}

void AlarmController::processMotionStatus(bool motionDetected)
{
    motionDetected_ = motionDetected;
}

void AlarmController::onMotionUpdate(const MotionData& data)
{
    processMotionStatus(data.detected);
}

void AlarmController::onAlarmTimerTimeout()
{
    if (!alarmSystem_) return;
    
    // Start alarm response timing
    alarmResponseTimer_->start();
    
    try {
        if (!motionDetected_) {
            handleNoMotionAlarm();
        } else {
            handleMotionDetectedAlarm();
            motionDetected_ = false; // Reset for next cycle
        }
        
        // Record alarm response performance
        double alarmResponseTime = alarmResponseTimer_->elapsedMs();
        if (perfMonitor_) {
            perfMonitor_->recordLatency("AlarmController", "AlarmResponse", alarmResponseTime);
        }
    } catch (const std::exception& e) {
        handleError(QString("Alarm processing failed: %1").arg(e.what()));
    }
}

void AlarmController::triggerMotionAlert()
{
    try {
        led_.blinkLED(BabyMonitorConfig::LED_BLINK_COUNT,
                     BabyMonitorConfig::LED_ON_DURATION_MS,
                     BabyMonitorConfig::LED_OFF_DURATION_MS);
        
        emit ledStatusChanged(true);
        errorHandler_.reportInfo("AlarmController", "Motion alert LED triggered");
    } catch (const std::exception& e) {
        handleError(QString("Failed to trigger LED alert: %1").arg(e.what()));
    }
}

void AlarmController::playAlarmSound()
{
    if (!audioPlayer_) return;
    
    try {
        audioPlayer_->play();
        alarmPlaying_ = true;
        errorHandler_.reportInfo("AlarmController", "Alarm sound started");
    } catch (const std::exception& e) {
        handleError(QString("Failed to play alarm sound: %1").arg(e.what()));
    }
}

void AlarmController::onAudioPlayerStateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState) {
        alarmPlaying_ = false;
        errorHandler_.reportInfo("AlarmController", "Alarm sound stopped");
    }
}

void AlarmController::initializeLED()
{
    // LED controller initializes itself
    errorHandler_.reportInfo("AlarmController", "LED controller initialized");
}

void AlarmController::initializeAudioPlayer()
{
    audioPlayer_ = new QMediaPlayer(this);
    audioPlayer_->setMedia(QUrl::fromLocalFile(BabyMonitorConfig::ALARM_SOUND_FILE));
    
    connect(audioPlayer_, &QMediaPlayer::stateChanged,
            this, &AlarmController::onAudioPlayerStateChanged);
    
    errorHandler_.reportInfo("AlarmController", "Audio player initialized");
}

void AlarmController::initializeAlarmTimer()
{
    alarmTimer_ = new QTimer(this);
    connect(alarmTimer_, &QTimer::timeout,
            this, &AlarmController::onAlarmTimerTimeout);
    
    errorHandler_.reportInfo("AlarmController", "Alarm timer initialized");
}

void AlarmController::publishAlarmMessage(const QString& message, int severity)
{
    if (alarmSystem_) {
        alarmSystem_->publishAlarm(message, severity);
        emit alarmTriggered(message, severity);
    }
}

void AlarmController::handleNoMotionAlarm()
{
    noMotionCount_++;
    QString message = QString("No motion detected !!!! Dangerous! (Sample #%1, Count: %2)")
                     .arg(samplesSent_++).arg(noMotionCount_);
    publishAlarmMessage(message, 3); // High severity
    
    // Play alarm sound after reaching threshold
    if (noMotionCount_ >= BabyMonitorConfig::NO_MOTION_ALARM_THRESHOLD && !alarmPlaying_) {
        playAlarmSound();
        triggerMotionAlert();
    }
}

void AlarmController::handleMotionDetectedAlarm()
{
    noMotionCount_ = 0; // Reset counter when motion is detected
    QString message = QString("On motion !!! (Sample #%1)").arg(samplesSent_++);
    publishAlarmMessage(message, 1); // Low severity
}

} // namespace BabyMonitor

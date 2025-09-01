// PerformanceController.cpp - Performance monitoring controller implementation
#include "PerformanceController.h"
#include <QDebug>
#include <QKeyEvent>

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

PerformanceController::PerformanceController(QLabel* performanceLabel, QObject* parent)
    : IUIController(parent)
    , perfMonitor_(&BabyMonitor::PerformanceMonitor::getInstance())
    , performanceReportTimer_(nullptr)
    , performanceLabel_(performanceLabel)
    , errorHandler_(BabyMonitor::ErrorHandler::getInstance())
    , lastFrameProcessingTime_(0.0)
    , performanceAlertCount_(0)
{
}

PerformanceController::~PerformanceController()
{
    stop();
}

bool PerformanceController::initialize()
{
    if (isInitialized_) return true;
    
    errorHandler_.reportInfo("PerformanceController", "Initializing performance controller");
    
    try {
        setupPerformanceReporting();
        
        isInitialized_ = true;
        errorHandler_.reportInfo("PerformanceController", "Initialized successfully");
        errorHandler_.reportInfo("PerformanceTest", "HOTKEYS: P=Report, A=Adapt, R=Reset");
        return true;
    } catch (const std::exception& e) {
        errorHandler_.reportError("PerformanceController", "Initialization failed", e.what());
        return false;
    }
}

void PerformanceController::start()
{
    if (!isInitialized_) {
        errorHandler_.reportError("PerformanceController", "Cannot start - not initialized");
        return;
    }
    
    if (isRunning_) return;
    
    try {
        if (performanceReportTimer_) {
            performanceReportTimer_->start(BabyMonitorConfig::PERFORMANCE_CHECK_INTERVAL_MS);
        }
        
        isRunning_ = true;
        updatePerformanceDisplay();
        errorHandler_.reportInfo("PerformanceController", "Started successfully");
        emit statusChanged("Performance: Monitoring");
    } catch (const std::exception& e) {
        errorHandler_.reportError("PerformanceController", "Failed to start", e.what());
        emit errorOccurred("PerformanceController", "Failed to start");
    }
}

void PerformanceController::stop()
{
    if (!isRunning_) return;
    
    try {
        if (performanceReportTimer_) {
            performanceReportTimer_->stop();
        }
        
        isRunning_ = false;
        errorHandler_.reportInfo("PerformanceController", "Stopped successfully");
        emit statusChanged("Performance: Stopped");
    } catch (const std::exception& e) {
        errorHandler_.reportError("PerformanceController", "Error stopping", e.what());
    }
}

bool PerformanceController::isHealthy() const
{
    return isInitialized_ && isRunning_ && perfMonitor_;
}

void PerformanceController::updateDisplay()
{
    updatePerformanceDisplay();
}

void PerformanceController::handleError(const QString& message)
{
    errorHandler_.reportError("PerformanceController", message);
    emit errorOccurred("PerformanceController", message);
}

void PerformanceController::handleKeyPress(QKeyEvent* event)
{
    if (!isRunning_) return;
    
    processPerformanceHotkeys(event->key());
}

void PerformanceController::logPerformanceReport()
{
    if (!perfMonitor_) return;
    
    try {
        perfMonitor_->logPerformanceReport();
        QString report = perfMonitor_->generatePerformanceReport();
        emit performanceReportGenerated(report);
        
        errorHandler_.reportInfo("PerformanceController", "Performance report generated");
    } catch (const std::exception& e) {
        handleError(QString("Failed to generate performance report: %1").arg(e.what()));
    }
}

void PerformanceController::updatePerformanceDisplay()
{
    if (!performanceLabel_) return;
    
    try {
        QString displayText = generatePerformanceDisplayText();
        performanceLabel_->setText(displayText);
    } catch (const std::exception& e) {
        handleError(QString("Failed to update performance display: %1").arg(e.what()));
    }
}

void PerformanceController::resetPerformanceStats()
{
    if (perfMonitor_) {
        perfMonitor_->clearStats();
        performanceAlertCount_ = 0;
        lastFrameProcessingTime_ = 0.0;
        
        errorHandler_.reportInfo("PerformanceController", "Performance statistics reset");
        updatePerformanceDisplay();
    }
}

void PerformanceController::onPerformanceReportTimer()
{
    logPerformanceReport();
    updatePerformanceDisplay();
}

void PerformanceController::onFrameProcessed(double processingTimeMs)
{
    lastFrameProcessingTime_ = processingTimeMs;
    
    // Update display periodically (not every frame to avoid UI lag)
    static int frameCount = 0;
    if (++frameCount % 30 == 0) { // Update every 30 frames
        updatePerformanceDisplay();
    }
}

void PerformanceController::onPerformanceAlert(const QString& message)
{
    performanceAlertCount_++;
    errorHandler_.reportWarning("PerformanceController", 
        QString("Performance alert #%1: %2").arg(performanceAlertCount_).arg(message));
    updatePerformanceDisplay();
}

void PerformanceController::setupPerformanceReporting()
{
    performanceReportTimer_ = new QTimer(this);
    connect(performanceReportTimer_, &QTimer::timeout,
            this, &PerformanceController::onPerformanceReportTimer);
    
    errorHandler_.reportInfo("PerformanceController", "Performance reporting setup completed");
}

void PerformanceController::processPerformanceHotkeys(int key)
{
    switch (key) {
    case Qt::Key_P:
        // Press 'P' to show performance report
        logPerformanceReport();
        updatePerformanceDisplay();
        break;
    case Qt::Key_A:
        // Press 'A' to manually trigger adaptation
        errorHandler_.reportInfo("PerformanceTest", "Manual adaptation triggered");
        emit adaptationRequested("All");
        updatePerformanceDisplay();
        break;
    case Qt::Key_R:
        // Press 'R' to reset and recover
        resetPerformanceStats();
        errorHandler_.reportInfo("PerformanceTest", "System reset to normal mode");
        emit recoveryRequested("All");
        updatePerformanceDisplay();
        break;
    default:
        // Ignore other keys
        break;
    }
}

QString PerformanceController::generatePerformanceDisplayText()
{
    QString text = QString("Frame: %1ms | Alerts: %2")
                  .arg(lastFrameProcessingTime_, 0, 'f', 1)
                  .arg(performanceAlertCount_);
    
    if (perfMonitor_) {
        // Add performance level indicator
        double frameLevel = perfMonitor_->getPerformanceLevel("CameraController", "FrameProcessing");
        if (frameLevel > 0.8) {
            text += " | HIGH LOAD";
        } else if (frameLevel > 0.6) {
            text += " | MODERATE";
        } else {
            text += " | NORMAL";
        }
    }
    
    return text;
}

} // namespace BabyMonitor

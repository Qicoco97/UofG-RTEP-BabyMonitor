// PerformanceController.h - Performance monitoring controller
#pragma once

#include "IUIController.h"
#include "../../performance/PerformanceMonitor.h"
#include "../../utils/Config.h"
#include "../../utils/ErrorHandler.h"
#include <QTimer>
#include <QLabel>
#include <QKeyEvent>

namespace BabyMonitor {

/**
 * Performance controller - handles performance monitoring and adaptation
 * Single responsibility: Performance monitoring and system optimization
 */
class PerformanceController : public IUIController {
    Q_OBJECT

public:
    explicit PerformanceController(QLabel* performanceLabel = nullptr, QObject* parent = nullptr);
    ~PerformanceController();

    // IComponent interface
    bool initialize() override;
    void start() override;
    void stop() override;
    bool isRunning() const override { return isRunning_; }
    QString getName() const override { return "PerformanceController"; }
    bool isHealthy() const override;

    // IUIController interface
    void updateDisplay() override;
    void handleError(const QString& message) override;

    // Performance-specific methods
    void handleKeyPress(QKeyEvent* event);
    void logPerformanceReport();
    void updatePerformanceDisplay();
    void resetPerformanceStats();

public slots:
    void onPerformanceReportTimer();
    void onFrameProcessed(double processingTimeMs);
    void onPerformanceAlert(const QString& message);

signals:
    void performanceReportGenerated(const QString& report);
    void adaptationRequested(const QString& component);
    void recoveryRequested(const QString& component);

private:
    // Performance monitoring
    BabyMonitor::PerformanceMonitor* perfMonitor_;
    QTimer* performanceReportTimer_;
    QLabel* performanceLabel_;
    
    // Error handling
    BabyMonitor::ErrorHandler& errorHandler_;
    
    // Performance statistics
    double lastFrameProcessingTime_;
    int performanceAlertCount_;
    
    // Helper methods
    void setupPerformanceReporting();
    void processPerformanceHotkeys(int key);
    QString generatePerformanceDisplayText();
};

} // namespace BabyMonitor

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <memory>

// Controller includes
#include "controllers/CameraController.h"
#include "controllers/ChartController.h"
#include "controllers/SensorController.h"
#include "controllers/AlarmController.h"
#include "controllers/PerformanceController.h"

// Core interfaces
#include "../managers/AlarmSystem.h"
#include "../utils/SensorData.h"
#include "../utils/ErrorHandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Dependency injection methods (public for bootstrap access)
    void setAlarmSystem(std::shared_ptr<BabyMonitor::IAlarmSystem> alarmSystem);

    // Public accessors for system status (read-only)
    bool isMotionDetected() const;
    const BabyMonitor::SystemStatus& getSystemStatus() const;
    const BabyMonitor::TemperatureHumidityData& getLastTempHumData() const;
    const BabyMonitor::MotionData& getLastMotionData() const;
    
protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onControllerStatusChanged(const QString& status);
    void onControllerError(const QString& component, const QString& message);

private:
    std::unique_ptr<Ui::MainWindow> ui;

    // Controllers (single responsibility)
    std::unique_ptr<BabyMonitor::CameraController> cameraController_;
    std::unique_ptr<BabyMonitor::ChartController> chartController_;
    std::unique_ptr<BabyMonitor::SensorController> sensorController_;
    std::unique_ptr<BabyMonitor::AlarmController> alarmController_;
    std::unique_ptr<BabyMonitor::PerformanceController> performanceController_;

    // Error handling
    BabyMonitor::ErrorHandler& errorHandler_;

    // Helper methods
    void initializeControllers();
    void startAllControllers();
    void stopAllControllers();
    void connectControllerSignals();
};

#endif // MAINWINDOW_H

// MainWindow.cpp - Simplified main window with controller delegation
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
    , errorHandler_(BabyMonitor::ErrorHandler::getInstance())
{
    ui->setupUi(this);

    // Initialize all controllers
    initializeControllers();

    // Connect controller signals
    connectControllerSignals();

    // Start all controllers
    startAllControllers();

    errorHandler_.reportInfo("MainWindow", "Main window initialized with controller architecture");
}

MainWindow::~MainWindow()
{
    // Stop all controllers
    stopAllControllers();

    errorHandler_.reportInfo("MainWindow", "Main window destroyed");
}

// Controller initialization methods
void MainWindow::initializeControllers()
{
    try {
        // Initialize camera controller
        cameraController_ = std::make_unique<BabyMonitor::CameraController>(ui->cameraLabel, this);

        // Initialize chart controller
        chartController_ = std::make_unique<BabyMonitor::ChartController>(
            ui->thChartView, ui->motionChartView, this);

        // Initialize sensor controller
        sensorController_ = std::make_unique<BabyMonitor::SensorController>(
            ui->tempLabel, ui->humLabel, ui->motionStatusLabel, this);

        // Initialize alarm controller
        alarmController_ = std::make_unique<BabyMonitor::AlarmController>(this);

        // Initialize performance controller (performanceStatusLabel may not exist in UI)
        QLabel* perfLabel = ui->findChild<QLabel*>("performanceStatusLabel");
        performanceController_ = std::make_unique<BabyMonitor::PerformanceController>(
            perfLabel, this);

        // Initialize all controllers
        cameraController_->initialize();
        chartController_->initialize();
        sensorController_->initialize();
        alarmController_->initialize();
        performanceController_->initialize();

        errorHandler_.reportInfo("MainWindow", "All controllers initialized successfully");
    } catch (const std::exception& e) {
        errorHandler_.reportError("MainWindow", "Controller initialization failed", e.what());
        throw;
    }
}

void MainWindow::startAllControllers()
{
    try {
        if (cameraController_) cameraController_->start();
        if (chartController_) chartController_->start();
        if (sensorController_) sensorController_->start();
        if (alarmController_) alarmController_->start();
        if (performanceController_) performanceController_->start();

        errorHandler_.reportInfo("MainWindow", "All controllers started successfully");
    } catch (const std::exception& e) {
        errorHandler_.reportError("MainWindow", "Failed to start controllers", e.what());
    }
}

void MainWindow::stopAllControllers()
{
    try {
        if (performanceController_) performanceController_->stop();
        if (alarmController_) alarmController_->stop();
        if (sensorController_) sensorController_->stop();
        if (chartController_) chartController_->stop();
        if (cameraController_) cameraController_->stop();

        errorHandler_.reportInfo("MainWindow", "All controllers stopped successfully");
    } catch (const std::exception& e) {
        errorHandler_.reportError("MainWindow", "Error stopping controllers", e.what());
    }
}

void MainWindow::connectControllerSignals()
{
    // Connect camera controller signals
    if (cameraController_) {
        connect(cameraController_.get(), &BabyMonitor::CameraController::frameReady,
                sensorController_.get(), &BabyMonitor::SensorController::onFrameReady);
        connect(cameraController_.get(), &BabyMonitor::CameraController::frameProcessed,
                performanceController_.get(), &BabyMonitor::PerformanceController::onFrameProcessed);
    }

    // Connect sensor controller signals
    if (sensorController_) {
        connect(sensorController_.get(), &BabyMonitor::SensorController::temperatureHumidityUpdated,
                chartController_.get(), &BabyMonitor::ChartController::onTemperatureHumidityUpdate);
        connect(sensorController_.get(), &BabyMonitor::SensorController::motionUpdated,
                chartController_.get(), &BabyMonitor::ChartController::onMotionUpdate);
        connect(sensorController_.get(), &BabyMonitor::SensorController::motionUpdated,
                alarmController_.get(), &BabyMonitor::AlarmController::onMotionUpdate);
    }

    // Connect performance controller signals
    if (performanceController_) {
        connect(performanceController_.get(), &BabyMonitor::PerformanceController::adaptationRequested,
                this, [this](const QString& component) {
                    if (component == "All" || component == "Camera") {
                        if (cameraController_) cameraController_->adaptFrameProcessing();
                    }
                });
        connect(performanceController_.get(), &BabyMonitor::PerformanceController::recoveryRequested,
                this, [this](const QString& component) {
                    if (component == "All" || component == "Camera") {
                        if (cameraController_) cameraController_->recoverFrameProcessing();
                    }
                });
    }

    // Connect all controller status and error signals to main window
    auto controllers = {
        static_cast<BabyMonitor::IUIController*>(cameraController_.get()),
        static_cast<BabyMonitor::IUIController*>(chartController_.get()),
        static_cast<BabyMonitor::IUIController*>(sensorController_.get()),
        static_cast<BabyMonitor::IUIController*>(alarmController_.get()),
        static_cast<BabyMonitor::IUIController*>(performanceController_.get())
    };

    for (auto* controller : controllers) {
        if (controller) {
            connect(controller, &BabyMonitor::IUIController::statusChanged,
                    this, &MainWindow::onControllerStatusChanged);
            connect(controller, &BabyMonitor::IUIController::errorOccurred,
                    this, &MainWindow::onControllerError);
        }
    }

    errorHandler_.reportInfo("MainWindow", "Controller signals connected successfully");
}

// Simplified event handling - delegate to controllers
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Delegate performance hotkeys to performance controller
    if (performanceController_) {
        performanceController_->handleKeyPress(event);
    }

    QMainWindow::keyPressEvent(event);
}

// Controller signal handlers
void MainWindow::onControllerStatusChanged(const QString& status)
{
    // Update main window status or log as needed
    errorHandler_.reportInfo("MainWindow", QString("Controller status: %1").arg(status));
}

void MainWindow::onControllerError(const QString& component, const QString& message)
{
    errorHandler_.reportError("MainWindow", QString("Controller error in %1: %2").arg(component).arg(message));
}

// Dependency injection methods
void MainWindow::setAlarmSystem(std::shared_ptr<BabyMonitor::IAlarmSystem> alarmSystem)
{
    if (alarmController_) {
        alarmController_->setAlarmSystem(alarmSystem);
        errorHandler_.reportInfo("MainWindow", "Alarm system injected successfully");
    }
}

// Public accessors (delegate to controllers)
bool MainWindow::isMotionDetected() const
{
    return sensorController_ ? sensorController_->isMotionDetected() : false;
}

const BabyMonitor::SystemStatus& MainWindow::getSystemStatus() const
{
    static BabyMonitor::SystemStatus defaultStatus{};
    return sensorController_ ? sensorController_->getSystemStatus() : defaultStatus;
}

const BabyMonitor::TemperatureHumidityData& MainWindow::getLastTempHumData() const
{
    static BabyMonitor::TemperatureHumidityData defaultData{};
    return sensorController_ ? sensorController_->getLastTempHumData() : defaultData;
}

const BabyMonitor::MotionData& MainWindow::getLastMotionData() const
{
    static BabyMonitor::MotionData defaultData{};
    return sensorController_ ? sensorController_->getLastMotionData() : defaultData;
}
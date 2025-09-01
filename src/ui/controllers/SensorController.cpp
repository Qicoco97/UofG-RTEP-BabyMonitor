// SensorController.cpp - Sensor management controller implementation
#include "SensorController.h"
#include <QDebug>

namespace BabyMonitor {

SensorController::SensorController(QLabel* tempLabel, QLabel* humLabel, QLabel* motionLabel,
                                 QObject* parent)
    : IUIController(parent)
    , tempLabel_(tempLabel)
    , humLabel_(humLabel)
    , motionLabel_(motionLabel)
    , dhtWorker_(nullptr)
    , motionThread_(nullptr)
    , motionWorker_(nullptr)
    , errorHandler_(BabyMonitor::ErrorHandler::getInstance())
    , dht11ConsecutiveErrors_(0)
{
    // Initialize system status
    systemStatus_.cameraActive = false;
    systemStatus_.dht11Active = false;
    systemStatus_.motionDetectionActive = false;
    systemStatus_.alarmSystemActive = false;
}

SensorController::~SensorController()
{
    stop();
}

bool SensorController::initialize()
{
    if (isInitialized_) return true;
    
    errorHandler_.reportInfo("SensorController", "Initializing sensor controller");
    
    try {
        initializeDHT11Sensor();
        initializeMotionDetection();
        
        isInitialized_ = true;
        errorHandler_.reportInfo("SensorController", "Initialized successfully");
        return true;
    } catch (const std::exception& e) {
        errorHandler_.reportError("SensorController", "Initialization failed", e.what());
        return false;
    }
}

void SensorController::start()
{
    if (!isInitialized_) {
        errorHandler_.reportError("SensorController", "Cannot start - not initialized");
        return;
    }
    
    if (isRunning_) return;
    
    try {
        // Start DHT11 sensor
        if (dhtWorker_) {
            dhtWorker_->start();
            systemStatus_.dht11Active = true;
            errorHandler_.reportInfo("SensorController", "DHT11 sensor started");
        }
        
        // Start motion detection (thread starts automatically)
        if (motionThread_) {
            motionThread_->start();
            systemStatus_.motionDetectionActive = true;
            errorHandler_.reportInfo("SensorController", "Motion detection started");
        }
        
        isRunning_ = true;
        updateSystemStatus();
        errorHandler_.reportInfo("SensorController", "All sensors started successfully");
        emit statusChanged("Sensors: Active");
    } catch (const std::exception& e) {
        errorHandler_.reportError("SensorController", "Failed to start sensors", e.what());
        emit errorOccurred("SensorController", "Failed to start sensors");
    }
}

void SensorController::stop()
{
    if (!isRunning_) return;
    
    try {
        cleanupDHT11Sensor();
        cleanupMotionDetection();
        
        systemStatus_.dht11Active = false;
        systemStatus_.motionDetectionActive = false;
        
        isRunning_ = false;
        updateSystemStatus();
        errorHandler_.reportInfo("SensorController", "All sensors stopped");
        emit statusChanged("Sensors: Stopped");
    } catch (const std::exception& e) {
        errorHandler_.reportError("SensorController", "Error stopping sensors", e.what());
    }
}

bool SensorController::isHealthy() const
{
    return isInitialized_ && isRunning_ && 
           systemStatus_.dht11Active && systemStatus_.motionDetectionActive;
}

void SensorController::updateDisplay()
{
    updateSensorLabels();
}

void SensorController::handleError(const QString& message)
{
    errorHandler_.reportError("SensorController", message);
    emit errorOccurred("SensorController", message);
}

void SensorController::onNewDHTReading(int t_int, int t_dec, int h_int, int h_dec)
{
    float temperature = t_int + t_dec / 100.0f;
    float humidity = h_int + h_dec / 100.0f;
    
    // Update structured sensor data
    lastTempHumData_ = TemperatureHumidityData(temperature, humidity, true);
    
    // Check if this is a recovery from error state
    bool wasInErrorState = (dht11ConsecutiveErrors_ > 0);
    
    // Reset consecutive error count on successful reading
    dht11ConsecutiveErrors_ = 0;
    systemStatus_.dht11Active = true;
    
    // Report successful reading
    static int readingCount = 0;
    readingCount++;
    
    if (wasInErrorState) {
        errorHandler_.reportInfo("DHT11", QString("Sensor recovered - Reading successful (T:%1°C, H:%2%)")
                                .arg(temperature, 0, 'f', 1).arg(humidity, 0, 'f', 1));
    } else if (readingCount % 20 == 0) {
        errorHandler_.reportInfo("DHT11", QString("Sensor stable - Reading #%1 (T:%2°C, H:%3%)")
                                .arg(readingCount).arg(temperature, 0, 'f', 1).arg(humidity, 0, 'f', 1));
    }
    
    updateSensorLabels();
    updateSystemStatus();
    emit temperatureHumidityUpdated(lastTempHumData_);
}

void SensorController::onDHTError()
{
    dht11ConsecutiveErrors_++;
    
    if (dht11ConsecutiveErrors_ >= DHT11_MAX_CONSECUTIVE_ERRORS) {
        systemStatus_.dht11Active = false;
        errorHandler_.reportError("DHT11", 
            QString("Sensor failed after %1 consecutive errors").arg(dht11ConsecutiveErrors_));
        emit sensorError("DHT11", "Multiple consecutive read failures");
    } else {
        errorHandler_.reportWarning("DHT11", 
            QString("Read error #%1/%2").arg(dht11ConsecutiveErrors_).arg(DHT11_MAX_CONSECUTIVE_ERRORS));
    }
    
    updateSystemStatus();
}

void SensorController::onMotionStatusChanged(bool detected)
{
    lastMotionData_ = MotionData(detected, detected ? 0.8 : 0.0);
    
    if (motionLabel_) {
        motionLabel_->setText(detected ? "Motion Detected" : "No Motion");
    }
    
    updateSystemStatus();
    emit motionUpdated(lastMotionData_);
}

void SensorController::onMotionWorkerPerformanceAlert(const QString& message)
{
    errorHandler_.reportWarning("MotionDetection", message);
    emit performanceAlert(message);
}

void SensorController::onFrameReady(const cv::Mat& frame)
{
    // Forward frame to motion worker for processing
    if (motionWorker_) {
        motionWorker_->processFrame(frame);
    }
}

void SensorController::initializeDHT11Sensor()
{
    // Create DHT11 sensor using factory
    dhtWorker_ = SensorFactory::createDHT11Worker(this);
    
    // Connect signals to controller slots
    connect(dhtWorker_, &DHT11Worker::newReading,
            this, &SensorController::onNewDHTReading);
    connect(dhtWorker_, &DHT11Worker::errorReading,
            this, &SensorController::onDHTError);
    
    errorHandler_.reportInfo("SensorController", "DHT11 sensor initialized");
}

void SensorController::initializeMotionDetection()
{
    // Create motion detection using factory
    auto motionSetup = SensorFactory::createMotionDetection(this);
    motionThread_ = motionSetup.thread;
    motionWorker_ = motionSetup.worker;
    
    // Setup connections using factory
    SensorFactory::connectMotionDetection(motionSetup, this, this);
    
    // Connect performance alert signal
    connect(motionWorker_, &MotionWorker::performanceAlert,
            this, &SensorController::onMotionWorkerPerformanceAlert);
    
    errorHandler_.reportInfo("SensorController", "Motion detection initialized");
}

void SensorController::cleanupDHT11Sensor()
{
    if (dhtWorker_) {
        dhtWorker_->stop();
        errorHandler_.reportInfo("SensorController", "DHT11 sensor stopped");
        // dhtWorker_ is a child of this, no need to delete
    }
}

void SensorController::cleanupMotionDetection()
{
    if (motionThread_) {
        motionThread_->quit();
        motionThread_->wait();
        delete motionThread_;
        motionThread_ = nullptr;
        motionWorker_ = nullptr; // Deleted with thread
        errorHandler_.reportInfo("SensorController", "Motion detection stopped");
    }
}

void SensorController::updateSensorLabels()
{
    if (tempLabel_) {
        tempLabel_->setText(QString::number(lastTempHumData_.temperature, 'f', 2) + " ℃");
    }
    if (humLabel_) {
        humLabel_->setText(QString::number(lastTempHumData_.humidity, 'f', 2) + " %");
    }
}

void SensorController::updateSystemStatus()
{
    systemStatus_.updateTimestamp();
    emit systemStatusChanged(systemStatus_);
}

} // namespace BabyMonitor

// CameraController.cpp - Camera management controller implementation
#include "CameraController.h"
#include <QDebug>

namespace BabyMonitor {

CameraController::CameraController(QLabel* displayLabel, QObject* parent)
    : IUIController(parent)
    , displayLabel_(displayLabel)
    , errorHandler_(BabyMonitor::ErrorHandler::getInstance())
    , perfMonitor_(&BabyMonitor::PerformanceMonitor::getInstance())
    , frameTimer_(new BabyMonitor::HighPrecisionTimer())
    , isFrameProcessingAdapted_(false)
    , frameSkipCounter_(0)
    , adaptiveFrameSkip_(1)
{
    cameraCallback_.controller = this;
}

CameraController::~CameraController()
{
    stop();
    delete frameTimer_;
}

bool CameraController::initialize()
{
    if (isInitialized_) return true;
    
    errorHandler_.reportInfo("CameraController", "Initializing camera controller");
    
    try {
        initializeCamera();
        setupPerformanceMonitoring();
        
        isInitialized_ = true;
        errorHandler_.reportInfo("CameraController", "Initialized successfully");
        return true;
    } catch (const std::exception& e) {
        errorHandler_.reportError("CameraController", "Initialization failed", e.what());
        return false;
    }
}

void CameraController::start()
{
    if (!isInitialized_) {
        errorHandler_.reportError("CameraController", "Cannot start - not initialized");
        return;
    }
    
    if (isRunning_) return;
    
    try {
        camera_.start();
        isRunning_ = true;
        errorHandler_.reportInfo("CameraController", "Started successfully");
        emit statusChanged("Camera: Running");
    } catch (const std::exception& e) {
        errorHandler_.reportError("CameraController", "Failed to start camera", e.what());
        emit errorOccurred("CameraController", "Failed to start camera");
    }
}

void CameraController::stop()
{
    if (!isRunning_) return;
    
    try {
        camera_.stop();
        isRunning_ = false;
        errorHandler_.reportInfo("CameraController", "Stopped successfully");
        emit statusChanged("Camera: Stopped");
    } catch (const std::exception& e) {
        errorHandler_.reportError("CameraController", "Error stopping camera", e.what());
    }
}

bool CameraController::isHealthy() const
{
    return isInitialized_ && isRunning_;
}

void CameraController::updateDisplay()
{
    // Display update is handled in updateImage method
    if (displayLabel_) {
        // Could add status information here if needed
    }
}

void CameraController::handleError(const QString& message)
{
    errorHandler_.reportError("CameraController", message);
    emit errorOccurred("CameraController", message);
}

void CameraController::updateImage(const cv::Mat& mat)
{
    if (!displayLabel_ || mat.empty()) return;
    
    try {
        const QImage frame(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        displayLabel_->setPixmap(QPixmap::fromImage(frame));
    } catch (const std::exception& e) {
        handleError(QString("Failed to update image: %1").arg(e.what()));
    }
}

void CameraController::processNewFrame(const cv::Mat& frame)
{
    if (frame.empty()) return;
    
    // Start frame processing timing
    frameTimer_->start();
    
    // Handle adaptive frame skipping for performance
    if (isFrameProcessingAdapted_) {
        frameSkipCounter_++;
        if (frameSkipCounter_ < adaptiveFrameSkip_) {
            return; // Skip this frame
        }
        frameSkipCounter_ = 0; // Reset counter
    }
    
    // Update camera display
    updateImage(frame);
    
    // Emit frame for motion detection processing
    emit frameReady(frame.clone());
    
    // Record frame processing performance
    double frameProcessingTime = frameTimer_->elapsedMs();
    emit frameProcessed(frameProcessingTime);
    
    if (perfMonitor_) {
        perfMonitor_->recordLatency("CameraController", "FrameProcessing", frameProcessingTime);
        
        // Check if frame processing adaptation is needed
        if (perfMonitor_->shouldAdaptPerformance("CameraController", "FrameProcessing")) {
            adaptFrameProcessing();
        } else if (isFrameProcessingAdapted_ && 
                   perfMonitor_->canRecoverPerformance("CameraController", "FrameProcessing")) {
            recoverFrameProcessing();
        }
    }
}

void CameraController::adaptFrameProcessing()
{
    if (isFrameProcessingAdapted_) return;
    
    isFrameProcessingAdapted_ = true;
    adaptiveFrameSkip_ = 2; // Skip every other frame
    frameSkipCounter_ = 0;
    
    errorHandler_.reportInfo("CameraController", "Adapted frame processing for performance");
    emit performanceAlert("Frame processing adapted - skipping frames for performance");
}

void CameraController::recoverFrameProcessing()
{
    if (!isFrameProcessingAdapted_) return;
    
    isFrameProcessingAdapted_ = false;
    adaptiveFrameSkip_ = 1;
    frameSkipCounter_ = 0;
    
    errorHandler_.reportInfo("CameraController", "Recovered frame processing to normal mode");
    emit statusChanged("Frame processing recovered to normal mode");
}

void CameraController::initializeCamera()
{
    camera_.registerCallback(&cameraCallback_);
    errorHandler_.reportInfo("CameraController", "Camera callback registered");
}

void CameraController::setupPerformanceMonitoring()
{
    // Initialize performance monitoring state
    isFrameProcessingAdapted_ = false;
    frameSkipCounter_ = 0;
    adaptiveFrameSkip_ = 1;
    
    errorHandler_.reportInfo("CameraController", "Performance monitoring setup completed");
}

void CameraController::onCameraError()
{
    handleError("Camera error occurred");
}

} // namespace BabyMonitor

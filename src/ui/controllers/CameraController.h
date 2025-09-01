// CameraController.h - Camera management controller
#pragma once

#include "IUIController.h"
#include "../../utils/Config.h"
#include "../../utils/ErrorHandler.h"
#include "../../performance/PerformanceMonitor.h"
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <memory>

// Include libcamera last and redefine Qt macros
#include "../../camera/libcam2opencv.h"

// Redefine Qt macros after libcamera (which undefines them)
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

/**
 * Camera controller - handles camera operations and frame processing
 * Single responsibility: Camera management and frame display
 */
class CameraController : public IUIController {
    Q_OBJECT

public:
    explicit CameraController(QLabel* displayLabel, QObject* parent = nullptr);
    ~CameraController();

    // IComponent interface
    bool initialize() override;
    void start() override;
    void stop() override;
    bool isRunning() const override { return isRunning_; }
    QString getName() const override { return "CameraController"; }
    bool isHealthy() const override;

    // IUIController interface
    void updateDisplay() override;
    void handleError(const QString& message) override;

    // Camera-specific methods
    void updateImage(const cv::Mat& mat);
    void processNewFrame(const cv::Mat& frame);

    // Performance adaptation
    void adaptFrameProcessing();
    void recoverFrameProcessing();

signals:
    void frameReady(const cv::Mat& frame);
    void frameProcessed(double processingTimeMs);

private slots:
    void onCameraError();

private:
    // Camera components
    Libcam2OpenCV camera_;
    QLabel* displayLabel_;
    
    // Performance monitoring
    BabyMonitor::ErrorHandler& errorHandler_;
    BabyMonitor::PerformanceMonitor* perfMonitor_;
    BabyMonitor::HighPrecisionTimer* frameTimer_;
    
    // Adaptive frame processing
    bool isFrameProcessingAdapted_;
    int frameSkipCounter_;
    int adaptiveFrameSkip_;

    // Camera callback implementation
    struct CameraCallbackImpl : Libcam2OpenCV::Callback {
        CameraController* controller = nullptr;
        
        void hasFrame(const cv::Mat& frame, const libcamera::ControlList&) override {
            if (controller) {
                controller->processNewFrame(frame);
            }
        }
    };
    
    CameraCallbackImpl cameraCallback_;
    
    // Helper methods
    void initializeCamera();
    void setupPerformanceMonitoring();
};

} // namespace BabyMonitor

// SensorFactory.h - Factory methods for sensor creation
#pragma once

#include <memory>
#include <QObject>
#include <QThread>
#include "DHT11/DHT11Worker.h"
#include "motionworker.h"
#include "Config.h"

namespace BabyMonitor {

/**
 * Sensor Factory - Follows Open/Closed Principle
 * Responsible for creating sensor instances with proper configuration
 */
class SensorFactory {
public:
    /**
     * Create DHT11 temperature/humidity sensor worker
     */
    static DHT11Worker* createDHT11Worker(QObject* parent = nullptr) {
        return new DHT11Worker(
            BabyMonitorConfig::GPIO_CHIP_DEVICE,
            BabyMonitorConfig::DHT11_PIN_NUMBER,
            parent,
            BabyMonitorConfig::DHT11_READ_INTERVAL_S
        );
    }

    /**
     * Create motion detection worker with thread
     */
    struct MotionDetectionSetup {
        QThread* thread;
        MotionWorker* worker;
    };

    static MotionDetectionSetup createMotionDetection(QObject* parent = nullptr) {
        QThread* motionThread = new QThread(parent);
        MotionWorker* worker = new MotionWorker(
            BabyMonitorConfig::MOTION_MIN_AREA,
            BabyMonitorConfig::MOTION_THRESHOLD
        );
        
        worker->moveToThread(motionThread);
        
        return {motionThread, worker};
    }

    /**
     * Setup motion detection connections
     */
    static void connectMotionDetection(const MotionDetectionSetup& setup, 
                                     QObject* frameSource, 
                                     QObject* motionReceiver) {
        // Connect thread lifecycle
        QObject::connect(setup.thread, &QThread::finished, 
                        setup.worker, &QObject::deleteLater);
        
        // Connect frame processing
        QObject::connect(frameSource, SIGNAL(frameReady(const cv::Mat&)), 
                        setup.worker, SLOT(processFrame(const cv::Mat&)));
        
        // Connect motion detection results
        QObject::connect(setup.worker, SIGNAL(motionDetected(bool)),
                        motionReceiver, SLOT(onMotionStatusChanged(bool)));
    }

private:
    SensorFactory() = default; // Static class, no instantiation
};

} // namespace BabyMonitor

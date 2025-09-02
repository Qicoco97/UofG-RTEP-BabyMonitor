# Motion Detection Module

## Overview

The Detection module is one of the core components of the UofG-RTEP-BabyMonitor project, responsible for real-time motion detection in video streams. This module uses OpenCV-based computer vision algorithms to provide intelligent motion sensing capabilities for the baby monitoring system.

## File Structure

```
src/detection/
├── motionworker.h      # MotionWorker class declaration file
└── motionworker.cpp    # MotionWorker class implementation file
```

## Core Components

### MotionWorker Class

`MotionWorker` is a worker class that inherits from `QObject`, specifically designed to execute motion detection tasks in a separate thread to avoid blocking the main UI thread.

#### Key Features

1. **Multi-threaded Processing**: Runs in a separate QThread to ensure UI responsiveness
2. **Adaptive Performance Optimization**: Dynamically adjusts detection parameters based on system performance
3. **Real-time Performance Monitoring**: Integrates high-precision timers and performance monitoring systems
4. **Signal-Slot Mechanism**: Communicates with other components through Qt signal-slot mechanism

#### Core Algorithm

Motion detection uses the classic Frame Difference algorithm:

1. **Preprocessing**: Convert color frames to grayscale images
2. **Gaussian Blur**: Use adjustable kernel size for noise filtering
3. **Frame Difference**: Calculate absolute difference between current and previous frames
4. **Binarization**: Apply threshold to convert difference image to binary image
5. **Morphological Operations**: Use dilation to connect adjacent motion regions
6. **Contour Detection**: Find and analyze contours of motion regions
7. **Area Filtering**: Filter small noise regions based on minimum area threshold

#### Performance Adaptive Mechanism

The system has intelligent performance adaptation capabilities:

**Performance Degradation Mode** (when performance bottlenecks are detected):
- Reduce Gaussian blur kernel size (21x21 → 15x15)
- Increase binarization threshold (thresh + 10)
- Increase minimum area threshold (minArea × 1.5)

**Performance Recovery Mode** (when system performance improves):
- Restore original parameter settings
- Provide optimal detection quality

## Integration in the Project

### 1. System Architecture Position

```
Camera (libcam2opencv) → MainWindow → MotionWorker → AlarmSystem
                                   ↓
                              Performance Monitor
```

### 2. Creation and Initialization

Created through the `SensorFactory` factory class:


### 3. Signal-Slot Connections

```cpp
// Frame processing connection
connect(frameSource, SIGNAL(frameReady(const cv::Mat&)), 
        motionWorker_, SLOT(processFrame(const cv::Mat&)));

// Motion detection result connection
connect(motionWorker_, SIGNAL(motionDetected(bool)),
        mainWindow, SLOT(onMotionStatusChanged(bool)));

// Performance alert connection
connect(motionWorker_, &MotionWorker::performanceAlert,
        mainWindow, &MainWindow::onMotionWorkerPerformanceAlert);
```

### 4. Configuration Parameters

Defined in `src/utils/Config.h`:

```cpp
constexpr double MOTION_MIN_AREA = 500.0;    // Minimum motion area
constexpr int MOTION_THRESHOLD = 25;         // Binarization threshold
```

## Performance Monitoring Integration

### Real-time Performance Metrics

- **Processing Latency**: Monitor processing time for each frame
- **Performance Threshold**: Maximum motion detection latency ≤ 50ms
- **Adaptive Trigger**: Trigger performance degradation when average latency exceeds 60% of threshold
- **Recovery Condition**: Recover to normal mode when latency drops below 40% of threshold

### Performance Monitoring API

```cpp
// Record processing time
perfMonitor_->recordLatency("MotionWorker", "MotionDetection", processingTime);
// Check if performance adaptation is needed
if (perfMonitor_->shouldAdaptPerformance("MotionWorker", "MotionDetection")) {
    adaptForPerformance();
}
```

## Interaction with Other Modules

### 1. Camera Module
- **Input**: Receives real-time video frames from `libcam2opencv`
- **Format**: BGR images in `cv::Mat` format

### 2. User Interface (UI)
- **Output**: Motion detection status updates UI labels
- **Display**: "On motion" / "No motion" status

### 3. Alarm System
- **Trigger**: Motion detection results are used to trigger alarm logic
- **Threshold**: Trigger alarm after 5 consecutive "No Motion" detections

### 4. LED Controller
- **Indication**: Controls LED blinking when motion is detected
- **Visual Feedback**: Provides intuitive status indication for users

### 5. DDS Communication
- **Publishing**: Motion detection results are published to network via DDS
- **Data Structure**: `BabyMonitor::MotionData`



## Technical Features

### Advantages

1. **High Performance**: Optimized OpenCV algorithm implementation
2. **Adaptive**: Dynamically adjusts parameters based on system load
3. **Real-time**: Meets real-time requirements for baby monitoring
4. **Extensible**: Modular design facilitates feature expansion
5. **Testable**: Provides testing interfaces and debugging features


## Dependencies

### External Dependencies
- **OpenCV**: Computer vision algorithm library
- **Qt**: Multi-threading and signal-slot mechanism

### Internal Dependencies
- **PerformanceMonitor**: Performance monitoring system
- **ErrorHandler**: Error handling system
- **Config**: Configuration management system





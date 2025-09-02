# Performance Monitoring Module

## Overview

The Performance module is the performance monitoring system of the UofG-RTEP-BabyMonitor project, responsible for real-time monitoring of performance metrics of system components to ensure the baby monitoring system meets real-time requirements. This module provides high-precision timing, performance statistics, constraint checking, and adaptive performance adjustment functions.

## Debugging Features

### Keyboard Shortcuts

- **P Key**: Display detailed performance report
- **A Key**: Manually trigger performance adaptation mode
- **R Key**: Reset performance statistics and restore normal mode

### Performance Display

UI interface displays key performance metrics in real-time:
- Motion detection latency and performance level
- Frame processing latency and performance level
- Alarm response latency and performance level

## Performance Monitoring Metrics

### Monitored Operation Types

1. **MotionDetection** - Motion detection (≤50ms)
2. **FrameProcessing** - Frame processing (≤20ms)
3. **AlarmResponse** - Alarm response (≤100ms)
4. **SensorReading** - Sensor reading (≤300ms)
5. **UIUpdate** - UI update (≤30ms)

### Performance Report

System automatically generates detailed performance reports:

```
=== Performance Monitor Report ===
MotionWorker::MotionDetection:
  Average: 15.2ms
  Min: 8.1ms, Max: 45.3ms
  Samples: 150
  Status: OK (30.4%)

MainWindow::FrameProcessing:
  Average: 12.8ms
  Min: 5.2ms, Max: 28.1ms
  Samples: 200
  Status: OK (64.0%)
```

## Core Components

### 1. RealTimeConstraints - Real-time Constraint Definition

Defines performance constraint thresholds for various system operations:

```cpp
namespace RealTimeConstraints {
    constexpr double MAX_MOTION_DETECTION_LATENCY_MS = 50.0;     // Maximum motion detection latency
    constexpr double MAX_ALARM_RESPONSE_LATENCY_MS = 100.0;      // Maximum alarm response latency
    constexpr double MAX_FRAME_PROCESSING_LATENCY_MS = 20.0;     // Maximum frame processing latency
    constexpr double MAX_SENSOR_READ_LATENCY_MS = 300.0;         // Maximum sensor reading latency
    constexpr double MAX_UI_UPDATE_LATENCY_MS = 30.0;            // Maximum UI update latency
    
    // Performance adaptation thresholds
    constexpr double PERFORMANCE_ADAPTATION_THRESHOLD = 0.6;     // Trigger degradation at 60%
    constexpr double PERFORMANCE_RECOVERY_THRESHOLD = 0.4;       // Recover to normal at 40%
}
```

### 2. HighPrecisionTimer - High-Precision Timer

Provides microsecond-level precision performance timing:

```cpp
class HighPrecisionTimer {
public:
    void start();                    // Start timing
    double elapsedMs() const;        // Get elapsed milliseconds
    double elapsedUs() const;        // Get elapsed microseconds
};
```

### 3. PerformanceStats - Performance Statistics

Collects and analyzes performance data:

```cpp
class PerformanceStats {
public:
    void addSample(double value);           // Add performance sample
    double getAverage() const;              // Get average value
    double getMin() const;                  // Get minimum value
    double getMax() const;                  // Get maximum value
    double getStdDev() const;               // Get standard deviation
    double getPercentile(double p) const;   // Get percentile
};
```

### 4. PerformanceMonitor - Performance Monitor

Core system performance monitoring component (Singleton pattern):

```cpp
class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance();
    // Record operation latency
    void recordLatency(const QString& component, const QString& operation, double latencyMs);
    // Check if performance adaptation is needed
    bool shouldAdaptPerformance(const QString& component, const QString& operation);
    // Check if performance can be recovered
    bool canRecoverPerformance(const QString& component, const QString& operation);
    // Get performance statistics
    const PerformanceStats* getStats(const QString& component, const QString& operation) const;
    // Generate performance report
    QString generatePerformanceReport() const;
};
```

## Usage in Project

### 1. Motion Detection Performance Monitoring

Monitor frame processing performance in MotionWorker:

### 2. Main Window Frame Processing Monitoring

Monitor frame processing and alarm response in MainWindow:

### 3. Alarm System Performance Monitoring

Monitor DDS publishing performance in AlarmSystem:


## Performance Adaptation Mechanism

### Adaptation Trigger Conditions

- **Performance Degradation**: Triggered when average latency exceeds 60% of limit
- **Performance Recovery**: Recovers when average latency drops below 40% of limit

### Adaptation Strategies

1. **Motion Detection Adaptation**:
   - Reduce Gaussian blur kernel size (21x21 → 15x15)
   - Increase binarization threshold
   - Increase minimum area threshold

2. **Frame Processing Adaptation**:
   - Enable frame skipping mechanism
   - Reduce processing frequency

3. **Alarm Publishing Adaptation**:
   - Adjust publishing frequency
   - Optimize message format


## Architectural Advantages

### 1. Real-time Monitoring

- Microsecond-level precision timing
- Real-time constraint checking
- Automatic performance violation reporting

### 2. Adaptive Optimization

- Intelligent performance degradation
- Automatic quality recovery
- Dynamic parameter adjustment

### 3. Statistical Analysis

- Complete performance statistics
- Percentile analysis
- Trend monitoring

### 4. Easy Integration

- Header-only design
- Singleton pattern access
- Minimized performance overhead


## Interaction with Other Modules

### System Architecture Position

```
All Components → PerformanceMonitor → Performance Statistics and Adaptation
       ↓                ↓                           ↓
   Timing            Constraint                Performance
  Measurement         Checking                 Adjustment
```

### Integrated Components

- **MotionWorker**: Motion detection performance monitoring
- **MainWindow**: Frame processing and UI performance monitoring
- **AlarmSystem**: Alarm publishing performance monitoring
- **ErrorHandler**: Performance violation reporting

---


# Performance Module

## Overview

The Performance module is the performance monitoring system of the UofG-RTEP-BabyMonitor project, responsible for real-time monitoring of performance metrics of system components to ensure the baby monitoring system meets real-time requirements. This module provides high-precision timing, performance statistics, constraint checking, and adaptive performance adjustment functions.

## Core Components

### 1. PerformanceMonitor - Performance Monitor

Core performance monitoring component of the system:

**Main Functions**:
- Real-time recording of performance metrics for each component
- Check whether performance degradation or recovery is needed
- Provide complete performance statistics and analysis
- Generate detailed performance reports

### 2. HighPrecisionTimer - High Precision Timer

Provides microsecond-level precision performance timing functionality:

**Main Functions**:
- Microsecond-level precision time measurement
- start() to begin timing, elapsedMs() to get milliseconds
- Minimize performance overhead

### 3. PerformanceStats - Performance Statistics

Collect and analyze performance data:

**Main Functions**:
- Statistical calculations: average, minimum, maximum, standard deviation
- Percentile analysis: support arbitrary percentile calculations
- Sample management: automatically manage performance sample data

### 4. RealTimeConstraints - Real-time Constraint Definitions

Define performance constraint thresholds for various system operations:

**Monitoring Metrics**:
- MotionDetection: Motion detection (≤50ms)
- FrameProcessing: Frame processing (≤20ms)
- AlarmResponse: Alarm response (≤100ms)
- SensorReading: Sensor reading (≤300ms)
- UIUpdate: UI update (≤30ms)

## Interaction with Other Modules

### detection/
- Monitor MotionWorker's motion detection performance to ensure real-time requirements
- Provide performance adaptation mechanisms to adjust detection parameters under high load

### ui/
- Monitor MainWindow's frame processing and UI update performance
- Provide performance debugging functions (P/A/R keys) and real-time performance display

### managers/
- Monitor AlarmSystem's alarm publishing performance
- Ensure alarm responses meet real-time requirements

### sensors/
- Monitor sensor reading performance, detect sensor response latency
- Provide sensor performance statistics and analysis

### core/
- Integrate into the system's unified error handling and logging mechanism
- Report performance violations and status changes through ErrorHandler






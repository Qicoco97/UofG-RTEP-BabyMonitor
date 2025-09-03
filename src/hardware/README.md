# Hardware Module

## Overview

The Hardware module is the hardware control component of the UofG-RTEP-BabyMonitor project, responsible for controlling LED indicator blinking behavior and managing other hardware devices. This module uses the libgpiod library to directly operate GPIO pins, providing visual status indication functionality for the baby monitoring system.

## Core Components

### 1. LEDController - LED Controller

A header-only C++ class that provides simple and reliable LED control functionality:

**Main Functions**:
- Use libgpiod library to directly control GPIO pins
- Execute blinking operations in background thread without blocking main program
- Open GPIO on construction, automatically release resources on destruction
- Use atomic variables to prevent concurrent blinking operations
- Use high-precision timing to control blinking intervals

**Core Methods**:
```cpp
// Constructor - Initialize GPIO
LEDController(int chipNo, int lineNo);

// Asynchronous blinking method
void blink(int n = 3, int onMs = 200, int offMs = 100);
```

### 2. Libcam2OpenCV - Camera Interface

Abstract interface for camera hardware, responsible for video stream capture and processing:

**Main Functions**:
- Camera initialization and configuration management
- Real-time video frame capture and format conversion
- Integration interface with OpenCV library
- Frame rate control and performance optimization

## Interaction with Other Modules

### ui/
- `LEDController` receives control commands from `MainWindow` to provide visual status feedback
- Triggers LED blinking alerts when motion anomalies are detected

### detection/
- `Libcam2OpenCV` provides real-time video frame data for `MotionWorker`
- Supports video stream processing requirements for motion detection algorithms

### managers/
- Controls LED status indication through `AlarmSystem` trigger mechanism
- Integrates into the overall alarm and notification system

### core/
- Reports hardware device status and error information through `ErrorHandler`
- Integrates into the system's unified error handling mechanism




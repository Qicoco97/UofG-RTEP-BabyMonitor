# UI Module

## Overview

The UI module is the user interface layer of the UofG-RTEP-BabyMonitor project, primarily composed of the MainWindow component. MainWindow serves as the core controller of the system, responsible for coordinating the operation of all subsystems, providing user interaction interfaces, and displaying monitoring data in real-time.

## Core Components

### 1. MainWindow - Main Window Controller

The core user interface component of the system, integrating all functional modules:

**Main Functions**:
- Display real-time footage captured by the camera
- Display temperature and humidity readings
- Display current motion detection status
- Real-time plotting of temperature and humidity change curves
- Display operational status of each component

**Performance Monitoring**:
- Monitor video frame processing latency
- Monitor alarm system response time
- Automatically adjust processing parameters based on performance
- Provide performance testing hotkeys (P/A/R keys)

## Interaction with Other Modules

### managers/
- Receives AlarmSystem services through dependency injection to implement alarm publishing functionality
- Integrates alarm system for processing motion detection results and network publishing

### sensors/
- Manages DHT11Worker for temperature and humidity data collection and display
- Coordinates sensor initialization, startup, and data update processes

### detection/
- Integrates MotionWorker for motion detection analysis
- Processes motion detection results and triggers corresponding alarms and LED control

### hardware/
- Controls LED indicator blinking behavior and status display
- Manages camera startup, shutdown, and frame processing

### core/
- Receives dependency-injected services through ServiceContainer
- Integrates ErrorHandler for error handling and status reporting







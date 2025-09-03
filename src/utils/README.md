# Utils Module

## Overview

The Utils module is the core utility library of the UofG-RTEP-BabyMonitor project, providing basic functions such as system configuration management, error handling, and data structure definitions. This module provides a unified configuration center, structured error management, and standardized data types for the entire project, serving as an important foundational component of the system architecture.

## Core Components

### 1. Config.h - Configuration Management Center

Centrally manages all system configuration constants, following the single configuration source principle:

**Main Functions**:
- Define GPIO pin numbers for hardware like DHT11, LED; configure sensor reading intervals, error thresholds and other parameters; set motion detection thresholds and algorithm parameters; define performance monitoring thresholds and adaptation parameters

### 2. ErrorHandler - Error Handling System

Provides unified error management and reporting mechanisms:

**Main Functions**:
- Unified error reporting interface
- Support multi-level error management (Info, Warning, Error, Critical)
- Maintain error history records and statistical information
- Automatically record to console and log files

### 3. SensorData.h - Data Structure Definitions

Defines standardized data structures used in the system:

**Main Data Structures**:
- `TemperatureHumidityData`: Temperature and humidity sensor data structure
- `MotionData`: Motion detection data structure
- `SystemStatus`: System status information structure
- `PerformanceMetrics`: Performance monitoring data structure

## Interaction with Other Modules

### core/
- `Config` provides configuration parameters for `ApplicationBootstrap` and `ServiceContainer`
- `ErrorHandler` integrates into the entire system's error handling mechanism

### sensors/
- `Config` provides GPIO configuration and reading parameters for DHT11 sensor
- `SensorData` defines standardized structures for sensor data

### detection/
- `Config` provides parameter configuration for motion detection algorithms
- `ErrorHandler` handles error reporting during motion detection processes

### ui/
- `SensorData` provides standardized data display structures for `MainWindow`
- `ErrorHandler` handles UI-related errors and status reporting

### hardware/
- `Config` provides configuration parameters for LED controller and other hardware devices
- `ErrorHandler` handles errors and status reporting for hardware devices

## Usage

### Configuration Management Usage

```cpp
// Use configuration constants in various components
#include "Config.h"

// DHT11 sensor configuration
auto dht11 = std::make_unique<DHT11Worker>(BabyMonitorConfig::DHT11_PIN_NUMBER);

// LED controller configuration
LEDController led(BabyMonitorConfig::LED_CHIP_NUMBER,
                  BabyMonitorConfig::LED_PIN_NUMBER);
```

### Error Handling Usage

```cpp
// Report errors in system components
#include "ErrorHandler.h"

ErrorHandler& errorHandler = ErrorHandler::getInstance();

// Report different levels of errors
errorHandler.reportError("DHT11 sensor initialization failed", ErrorLevel::Error);
errorHandler.reportError("Motion detection performance warning", ErrorLevel::Warning);
```

### Data Structure Usage

```cpp
// Use standardized data structures
#include "SensorData.h"

// Temperature and humidity data
BabyMonitor::TemperatureHumidityData tempHumData;
tempHumData.temperature = 25.5f;
tempHumData.humidity = 60.0f;
tempHumData.timestamp = QDateTime::currentDateTime();

// System status
BabyMonitor::SystemStatus status;
status.isRunning = true;
status.sensorsHealthy = true;
status.lastUpdate = QDateTime::currentDateTime();
```

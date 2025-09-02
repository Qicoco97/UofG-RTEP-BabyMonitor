# Sensors Module

## Overview

The Sensors module is the sensor management layer of the UofG-RTEP-BabyMonitor project, responsible for handling data collection and management of various physical sensors. This module provides temperature and humidity monitoring, sensor factory creation, and unified sensor management interfaces, providing environmental monitoring capabilities for the baby monitoring system.

## File Structure

```
src/sensors/
├── DHT11Gpio.h         # DHT11 sensor GPIO low-level driver
├── DHT11Gpio.cpp       # DHT11 sensor GPIO low-level implementation
├── DHT11Worker.h       # DHT11 sensor worker thread
├── DHT11Worker.cpp     # DHT11 sensor worker thread implementation
├── SensorFactory.h     # Sensor factory class
└── CMakeLists.txt      # Build configuration file
```

## Core Components

### 1. DHT11Gpio - DHT11 Low-level Driver

Responsible for low-level GPIO communication with DHT11 temperature and humidity sensor:

#### Key Functions

- **GPIO Control**: Direct GPIO pin manipulation using libgpiod library
- **Protocol Implementation**: Implementation of DHT11 single-bus communication protocol
- **Data Parsing**: Parse temperature/humidity data and checksum
- **Error Handling**: Handle communication timeouts and data validation errors


### 2. DHT11Worker - DHT11 Worker Thread

Provides asynchronous temperature and humidity data collection service:

#### Key Features

- **Multi-threaded Execution**: Runs in separate thread without blocking main program
- **Scheduled Collection**: Configurable data collection intervals
- **Signal Notification**: Sends data through Qt signal-slot mechanism
- **Error Recovery**: Automatically handles read errors and sensor failures


### 3. SensorFactory - Sensor Factory

Sensor creation factory following the Open/Closed Principle:


## Usage in Project

### 1. DHT11 Sensor Integration

Using DHT11 sensor in MainWindow:


### 2. Data Processing

Processing temperature and humidity data and updating UI:


### 3. Error Handling

Handling sensor read errors:


## Configuration Parameters

Define sensor-related configuration in `src/utils/Config.h`:

```cpp
namespace BabyMonitorConfig {
    // GPIO Configuration
    constexpr int DHT11_PIN_NUMBER = 17;                    // DHT11 pin number
    constexpr const char* GPIO_CHIP_DEVICE = "/dev/gpiochip0";  // GPIO device path
    
    // DHT11 Configuration
    constexpr int DHT11_READ_INTERVAL_S = 3;                // Read interval (seconds)
    constexpr int DHT11_MAX_CONSECUTIVE_ERRORS = 5;         // Maximum consecutive errors
}
```

## Sensor Management

### Lifecycle Management

Sensors follow unified lifecycle management:

1. **Initialization**: Create sensor instances through SensorFactory
2. **Configuration**: Connect signal-slots and set parameters
3. **Startup**: Start data collection thread
4. **Operation**: Continuous monitoring and data reporting
5. **Shutdown**: Graceful shutdown and resource cleanup

### Error Recovery Mechanism

- **Consecutive Error Counting**: Track consecutive failure count
- **Automatic Retry**: Sensor automatically attempts to re-read
- **State Recovery**: Reset error count after successful read
- **System Notification**: Report status changes through ErrorHandler


## Architectural Advantages

### 1. Modular Design

- **Layered Architecture**: Separation of low-level driver, worker thread, and factory creation
- **Clear Responsibilities**: Each component focuses on specific functionality
- **Easy Extension**: Can easily add new sensor types

### 2. Asynchronous Processing

- **Non-blocking**: Sensor reading performed in separate thread
- **Responsiveness**: Does not affect main UI thread responsiveness
- **Concurrency**: Multiple sensors can work simultaneously

### 3. Error Tolerance

- **Robustness**: Comprehensive error handling and recovery mechanisms
- **Stability**: Sensor failures do not affect other system functions
- **Monitoring**: Real-time monitoring of sensor health status

### 4. Factory Pattern

- **Unified Creation**: Unified sensor instance creation through factory
- **Configuration Management**: Centralized management of sensor configuration parameters
- **Memory Management**: Automatic memory management using Qt parent-child relationships

## Interaction with Other Modules

### System Architecture Position

```
Hardware Sensors → DHT11Gpio → DHT11Worker → MainWindow → UI Display
                                    ↓              ↓
                              SensorFactory → Chart Updates
```

### Data Flow

```
Physical Sensor → GPIO Read → Data Parse → Signal Send → UI Update
       ↓             ↓           ↓           ↓           ↓
     DHT11       DHT11Gpio   DHT11Worker   MainWindow   Charts
```

### Integrated Components

- **MainWindow**: Receives sensor data and updates UI
- **ErrorHandler**: Handles sensor errors and status reporting
- **Config**: Provides sensor configuration parameters
- **SensorManager**: Unified management of multiple sensors (optional)




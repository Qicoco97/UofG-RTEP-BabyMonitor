# Managers Module

## Overview

The Managers module is the business logic management layer of the UofG-RTEP-BabyMonitor project, responsible for implementing the system's core management functions. This module provides key services such as alarm management and sensor management by implementing interfaces defined in the interfaces module, serving as an important bridge connecting lower-level components and upper-level UI.


## Core Components

### 1. AlarmSystem - Alarm System Manager

Implements the `IAlarmSystem` interface, responsible for managing system alarm publishing and DDS communication functions.

#### Key Functions

- **Alarm Publishing**: Publishes alarm messages to network via DDS protocol
- **Message Formatting**: Formats alarm messages based on severity level
- **Performance Monitoring**: Integrates performance monitoring with adaptive publishing frequency
- **Lifecycle Management**: Complete initialization, start, and stop processes


#### Performance Adaptation

- **Adaptive Publishing Frequency**: Dynamically adjusts publishing intervals based on system performance
- **Performance Monitoring**: Records latency time for each publish operation
- **Intelligent Degradation**: Reduces publishing frequency under high load

### 2. SensorManager - Sensor Manager

Implements the `ISensorManager` interface, responsible for managing multiple sensor components in the system.

#### Key Functions

- **Sensor Registration**: Dynamically add and remove sensors
- **Lifecycle Management**: Unified management of all sensor startup and shutdown
- **Health Monitoring**: Real-time monitoring of sensor status
- **Error Handling**: Identify and report faulty sensors

#### Signal Mechanism

```cpp
signals:
    void sensorAdded(const QString& name);
    void sensorRemoved(const QString& name);
    void sensorStatusChanged(const QString& name, bool isHealthy);
    void allSensorsHealthy();
    void sensorsWithErrors(const QStringList& errorSensors);
```


## Dependency Injection Integration

Dependency injection through ServiceContainer:

```cpp
// Register service in main.cpp
container.registerService<BabyMonitor::IAlarmSystem>("AlarmSystem", []() {
    return std::make_shared<BabyMonitor::AlarmSystem>();
});

// Use in MainWindow
auto alarmSystem = container.resolve<BabyMonitor::IAlarmSystem>("AlarmSystem");
mainWindow.setAlarmSystem(alarmSystem);
```

### Alarm System Usage

```cpp
// In MainWindow's timerEvent
if (injectedAlarmSystem_) {
    if (!motionDetected_) {
        QString message = QString("No motion detected !!!! Dangerous!");
        injectedAlarmSystem_->publishAlarm(message, 3); // High severity
    } else {
        QString message = QString("On motion !!!");
        injectedAlarmSystem_->publishAlarm(message, 1); // Low severity
    }
}
```

### Sensor Management Usage

```cpp
// Add sensors to manager
sensorManager->addSensor("DHT11", dht11Worker);
sensorManager->addSensor("MotionDetector", motionWorker);

// Start all sensors
sensorManager->start();

// Check sensor health status
if (!sensorManager->areAllSensorsHealthy()) {
    QStringList errorSensors = sensorManager->getErrorSensors();
    // Handle faulty sensors...
}
```

## Architectural Advantages

### 1. Interface-Driven Design

- Implements standardized interfaces ensuring loose coupling between components
- Supports dependency injection, improving code testability
- Easy to replace implementations, enhancing system flexibility

### 2. Unified Management

- **AlarmSystem**: Centralized management of all alarm logic
- **SensorManager**: Unified management of sensor lifecycle
- Simplifies upper-layer calls, reducing system complexity

### 3. Performance Optimization

- Integrates performance monitoring system
- Supports adaptive performance adjustment
- Real-time monitoring of component health status

### 4. Error Handling

- Comprehensive error reporting mechanism
- Unified error handling through ErrorHandler
- Supports graceful degradation and recovery


## Interaction with Other Modules

### System Architecture Position

```
UI Layer (MainWindow)
        ↓ (Dependency Injection)
Managers Layer (AlarmSystem, SensorManager)
        ↓ (Interface Calls)
Communication Layer (DDS) + Sensors Layer
```

### Data Flow

```
Sensor Data → MainWindow → AlarmSystem → DDS Network
Sensor Components → SensorManager → Health Status Monitoring
```



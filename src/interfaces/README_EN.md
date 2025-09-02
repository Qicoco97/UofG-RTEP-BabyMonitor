# Interfaces Module

## Overview

The Interfaces module defines the core interfaces for the UofG-RTEP-BabyMonitor project, following the Interface Segregation Principle to provide standardized abstract interfaces for system components. This module is the foundation for implementing dependency injection and loose coupling architecture.


## Core Interfaces

### 1. IComponent - Base Component Interface

The base interface for all system components, defining component lifecycle management:

```cpp
class IComponent {
public:
    virtual bool initialize() = 0;      // Initialize component
    virtual void start() = 0;           // Start component
    virtual void stop() = 0;            // Stop component
    virtual bool isRunning() const = 0; // Check running status
    virtual QString getName() const = 0; // Get component name
    virtual bool isHealthy() const = 0;  // Check health status
};
```

### 2. ISensorManager - Sensor Management Interface

Interface for managing multiple sensors as a group:

```cpp
class ISensorManager : public IComponent {
public:
    virtual bool addSensor(const QString& name, IComponent* sensor) = 0;
    virtual bool removeSensor(const QString& name) = 0;
    virtual IComponent* getSensor(const QString& name) const = 0;
    virtual QStringList getActiveSensors() const = 0;
    virtual QStringList getErrorSensors() const = 0;
    virtual bool areAllSensorsHealthy() const = 0;
};
```

### 3. IAlarmSystem - Alarm System Interface

Interface for handling alarm publishing and management:

```cpp
class IAlarmSystem : public IComponent {
public:
    virtual bool publishAlarm(const QString& message, int severity = 1) = 0;
    virtual bool hasSubscribers() const = 0;
    virtual void setPublishInterval(int intervalMs) = 0;
};
```

### 4. IDisplayManager - Display Management Interface

Interface for handling UI updates and chart management:

```cpp
class IDisplayManager : public IComponent {
public:
    virtual void updateTemperatureDisplay(float temperature) = 0;
    virtual void updateHumidityDisplay(float humidity) = 0;
    virtual void updateMotionDisplay(bool detected) = 0;
    virtual void updateSystemStatus(const QString& status) = 0;
    virtual void showError(const QString& message) = 0;
};
```

## Interface Implementations

### Implemented Interfaces

1. **AlarmSystem** - Implements `IAlarmSystem` interface
   - Location: `src/managers/AlarmSystem.h/cpp`
   - Function: DDS communication and alarm logic management

2. **SensorManager** - Implements `ISensorManager` interface
   - Location: `src/managers/SensorManager.h/cpp`
   - Function: Sensor group management and health monitoring

## Usage in Project

### Dependency Injection Pattern

Implement dependency injection through interfaces to reduce coupling between components:

```cpp
// Using interface in MainWindow
std::shared_ptr<BabyMonitor::IAlarmSystem> injectedAlarmSystem_;

// Set implementation through dependency injection
void setAlarmSystem(std::shared_ptr<IAlarmSystem> alarmSystem) {
    injectedAlarmSystem_ = alarmSystem;
}

// Use interface methods
if (injectedAlarmSystem_) {
    injectedAlarmSystem_->publishAlarm(message, severity);
}
```

### Service Container Integration

Used in conjunction with the core module's ServiceContainer:

```cpp
// Register service
container.registerService<IAlarmSystem>("AlarmSystem", 
    std::make_shared<AlarmSystem>());

// Resolve service
auto alarmSystem = container.resolve<IAlarmSystem>("AlarmSystem");
```

## Architectural Advantages

### 1. Loose Coupling
- Components communicate through interfaces
- Easy to replace implementations
- Improves code flexibility

### 2. Testability
- Easy to create Mock objects
- Supports unit testing
- Isolates testing environment

### 3. Extensibility
- New components only need to implement interfaces
- Does not affect existing code
- Supports plugin-style architecture




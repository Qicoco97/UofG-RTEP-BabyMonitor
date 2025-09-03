# Interfaces Module

## Overview

The Interfaces module defines the core interfaces for the UofG-RTEP-BabyMonitor project, following the Interface Segregation Principle to provide standardized abstract interfaces for system components. This module is the foundation for implementing dependency injection and loose coupling architecture.

## Core Components

### 1. IComponent - Base Component Interface

The base interface for all system components, defining component lifecycle management

### 2. ISensorManager - Sensor Management Interface

Interface for managing multiple sensors as a group

### 3. IAlarmSystem - Alarm System Interface

Interface for handling alarm publishing and management

### 4. IDisplayManager - Display Management Interface

Interface for handling UI updates and chart management


## Interaction with Other Modules

### managers/
- `AlarmSystem` implements the `IAlarmSystem` interface, responsible for DDS communication and alarm logic management
- `SensorManager` implements the `ISensorManager` interface, responsible for sensor group management and health monitoring

### ui/
- `MainWindow` receives `IAlarmSystem` services through dependency injection, achieving decoupling between UI and business logic

### core/
- `ServiceContainer` performs service registration and resolution through these interfaces, implementing type-safe dependency management

## Usage

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

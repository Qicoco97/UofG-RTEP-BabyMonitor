# Managers Module

## Overview

The Managers module is the business logic management layer of the UofG-RTEP-BabyMonitor project, responsible for implementing the system's core management functions. This module provides key services such as alarm management and sensor management by implementing interfaces defined in the interfaces module, serving as an important bridge connecting lower-level components and upper-level UI.

## Core Components

### 1. AlarmSystem - Alarm System Manager

Implements the `IAlarmSystem` interface, responsible for managing system alarm publishing and DDS communication functions.

**Main Functions**:
- Publishes alarm messages to network via DDS protocol; formats alarm messages according to severity levels; integrates performance monitoring with adaptive publishing frequency support; complete initialization, startup, and shutdown processes

**Performance Adaptation**:
- Dynamically adjusts publishing intervals based on system performance; records latency time for each publish; reduces publishing frequency under high load conditions

### 2. SensorManager - Sensor Manager

Implements the `ISensorManager` interface, responsible for managing multiple sensor components in the system.

**Main Functions**:
- Dynamically adds and removes sensors; uniformly manages startup and shutdown of all sensors; real-time monitoring of sensor status; identifies and reports faulty sensors

## Interaction with Other Modules

### interfaces/
- AlarmSystem implements the IAlarmSystem interface, providing standardized alarm services
- SensorManager implements the ISensorManager interface, providing sensor management services

### ui/
- MainWindow receives AlarmSystem services through dependency injection, implementing alarm publishing functionality
- Uniformly manages sensor lifecycle and health status through SensorManager

### communication/
- AlarmSystem uses the DDS communication layer to publish alarm messages to the network
- Supports real-time message delivery and network communication

### core/
- Performs service registration and dependency injection through ServiceContainer
- Integrates ErrorHandler for unified error handling and log management




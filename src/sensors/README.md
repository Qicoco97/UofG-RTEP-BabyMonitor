# Sensors Module

## Overview

The Sensors module is the sensor management layer of the UofG-RTEP-BabyMonitor project, responsible for handling data collection and management of various physical sensors. This module provides temperature and humidity monitoring, sensor factory creation, and unified sensor management interfaces, providing environmental monitoring capabilities for the baby monitoring system.

## Core Components

### 1. DHT11Gpio - DHT11 Low-level Driver

Responsible for low-level GPIO communication of DHT11 temperature and humidity sensor:

**Main Functions**:
- Use libgpiod library to directly operate GPIO pins
- Implement DHT11 single-bus communication protocol
- Parse temperature and humidity data and checksums
- Handle communication timeouts and data verification errors

### 2. DHT11Worker - DHT11 Worker Thread

Provides asynchronous temperature and humidity data collection services:

**Main Functions**:
- Execute in independent thread without blocking main program
- Configurable data collection interval (default 3 seconds)
- Send data through Qt signal-slot mechanism
- Automatically handle read errors and sensor failures

### 3. SensorFactory - Sensor Factory

Sensor creation factory following the open-closed principle:

**Main Functions**:
- Provide unified creation methods for different types of sensors
- Support dependency injection and lifecycle management of sensors
- Facilitate adding new sensor types

## Interaction with Other Modules

### ui/
- `DHT11Worker` sends temperature and humidity data to `MainWindow`, updating UI display and charts
- Implements real-time data transmission and visualization through signal-slot mechanism

### managers/
- Unified sensor lifecycle management through `SensorManager`
- Integrates sensor health monitoring and error reporting mechanisms

### core/
- Sensor service registration and dependency injection through `ServiceContainer`
- Integrates `ErrorHandler` for sensor error handling and logging

### utils/
- Uses `Config` module for sensor parameter configuration management
- Integrates `SensorData` data structures for standardized data transmission








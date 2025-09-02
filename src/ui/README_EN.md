# UI Module

## Overview

The UI module is the user interface layer of the UofG-RTEP-BabyMonitor project, primarily composed of the MainWindow component. MainWindow serves as the core controller of the system, responsible for coordinating the operation of all subsystems, providing user interaction interfaces, and displaying monitoring data in real-time.

## MainWindow Core Functions

### 1. System Integration Center

MainWindow is the integration center of the entire baby monitoring system, responsible for:

- **Sensor Management**: Initialize and manage DHT11 temperature and humidity sensors
- **Camera Control**: Manage camera startup, shutdown, and frame processing
- **Motion Detection**: Coordinate the work of motion detection modules
- **Alarm System**: Integrate alarm publishing and audio playback functions
- **LED Control**: Manage LED indicator blinking behavior

### 2. User Interface Management

Provides intuitive user interface display:

- **Real-time Video**: Display real-time footage captured by the camera
- **Sensor Data**: Display temperature and humidity readings
- **Motion Status**: Display current motion detection status
- **Data Charts**: Real-time plotting of temperature and humidity change curves
- **System Status**: Display the operational status of each component

### 3. Data Flow Coordination

Acts as the central processor for data flow:

```
Camera → MainWindow → Motion Detection
Sensors → MainWindow → UI Display + Chart Updates
Motion Detection Results → MainWindow → Alarm System + LED Control
```

### 4. Performance Monitoring

Integrates performance monitoring functions:

- **Frame Processing Performance**: Monitor video frame processing latency
- **Alarm Response Performance**: Monitor alarm system response time
- **Adaptive Adjustment**: Automatically adjust processing parameters based on performance
- **Debug Functions**: Provide performance testing hotkeys (P/A/R keys)

## Main Functional Modules

### Sensor Integration

- **DHT11 Sensor**: Temperature and humidity data collection and display
- **Error Handling**: Sensor fault detection and recovery
- **Data Validation**: Sensor data validity checking

### Camera and Motion Detection

- **Video Stream Processing**: Real-time video display and frame distribution
- **Motion Detection Integration**: Collaborative work with MotionWorker
- **Performance Optimization**: Frame skipping and adaptive processing

### Alarms and Notifications

- **DDS Alarms**: Publish alarm messages over the network
- **Audio Alarms**: Play alarm sound files
- **LED Indication**: Control LED blinking to provide visual feedback
- **Threshold Management**: Manage no-motion alarm thresholds

### Charts and Data Visualization

- **Real-time Charts**: Use Qt Charts to display temperature and humidity curves
- **Data Management**: Manage chart data point count and display range
- **Time Axis**: Provide time series data display

## Dependency Injection Support

MainWindow supports dependency injection pattern:

```cpp
// Accept externally injected alarm system
void setAlarmSystem(std::shared_ptr<BabyMonitor::IAlarmSystem> alarmSystem);

// Provide system status access interface
const BabyMonitor::SystemStatus& getSystemStatus() const;
const BabyMonitor::TemperatureHumidityData& getLastTempHumData() const;
```

## Error Handling and Status Management

- **Unified Error Handling**: Integrate ErrorHandler for error reporting
- **System Status Tracking**: Maintain operational status of each component
- **Fault Recovery**: Handle sensor and component failures
- **Status Updates**: Real-time UI status display updates

## Performance Features

- **Multi-threading Support**: Sensors and motion detection run in separate threads
- **Adaptive Performance**: Adjust processing strategies based on system load
- **Memory Management**: Use Qt parent-child relationships for automatic memory management
- **Resource Optimization**: Intelligent frame processing and data management


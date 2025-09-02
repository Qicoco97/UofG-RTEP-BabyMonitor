# Utils Module

## Overview

The Utils module is the core utility library of the UofG-RTEP-BabyMonitor project, providing basic functions such as system configuration management, error handling, and data structure definitions. This module provides a unified configuration center, structured error management, and standardized data types for the entire project, serving as an important foundational component of the system architecture.


## Core Components

### 1. Config.h - Configuration Management Center

Centrally manages all system configuration constants, following the single configuration source principle


### 2. ErrorHandler - Error Handling System

Provides unified error management and reporting mechanisms

#### Core Functions

- **Error Reporting**: Unified error reporting interface
- **Error Classification**: Supports multi-level error management
- **History Recording**: Maintains error history records
- **Console Output**: Automatically logs to console


### 3. SensorData.h - Data Structure Definition

Defines standardized data structures used in the system:

TemperatureHumidityData, MotionData, SystemStatus


## Usage in Project

### 1. Configuration Management Usage

Using configuration constants in various components:


### 2. Error Handling Usage

Reporting errors in system components:


### 3. Data Structure Usage

Using standardized data structures:



## Design Advantages

### 1. Centralized Configuration Management

- **Single Configuration Source**: All configurations centralized in Config.h
- **Compile-time Constants**: Uses constexpr to ensure compile-time optimization
- **Type Safety**: Strongly typed configuration parameters
- **Easy Maintenance**: Configuration changes only require updating one file

### 2. Unified Error Handling

- **Standardized Interface**: Unified error reporting methods
- **Hierarchical Management**: Supports different levels of error handling
- **History Tracking**: Complete error history records
- **Singleton Pattern**: Globally unified error handling instance

### 3. Structured Data

- **Type Safety**: Strongly typed data structures
- **Data Validation**: Built-in data validity checking
- **Backward Compatibility**: Maintains backward compatibility of public member variables
- **Encapsulation Improvement**: Provides getter/setter methods

### 4. Modular Design

- **Separation of Concerns**: Each file focuses on specific functionality
- **Low Coupling**: Minimizes dependencies between modules
- **High Cohesion**: Related functions centrally managed
- **Easy Extension**: Convenient for adding new configurations and data types


## Interaction with Other Modules

### System Architecture Position

```
All Modules → Utils (Config, ErrorHandler, SensorData)
     ↓              ↓
Configuration    Error Reporting + Data Structures
  Retrieval
```

### Using Modules

- **MainWindow**: Uses all utils components
- **Sensors**: Uses Config and SensorData
- **Hardware**: Uses Config to configure GPIO parameters
- **Performance**: Uses Config to configure monitoring parameters
- **Managers**: Uses ErrorHandler and data structures



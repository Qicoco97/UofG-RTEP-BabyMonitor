# LED Controller Module

## Overview

The LED Controller module is a hardware control component of the UofG-RTEP-BabyMonitor project, responsible for controlling LED indicator blinking behavior. This module uses the libgpiod library to directly operate GPIO pins, providing visual status indication functionality for the baby monitoring system.


## Core Functionality

### LEDController Class

`LEDController` is a header-only C++ class that provides simple and reliable LED control functionality.

#### Key Features

1. **GPIO Control**: Uses libgpiod library to directly control GPIO pins
2. **Asynchronous Blinking**: Executes blinking operations in background thread without blocking main program
3. **Resource Management**: Opens GPIO on construction, automatically releases resources on destruction
4. **Thread Safety**: Uses atomic variables to prevent concurrent blinking operations
5. **Precise Timing**: Uses high-precision timing to control blinking intervals

#### Core Methods

```cpp
// Constructor - Initialize GPIO
LEDController(int chipNo, int lineNo);

// Asynchronous blinking method
void blink(int n = 3, int onMs = 200, int offMs = 100);
```

## Usage in Project

### 1. Initialization

Initialize LED controller in MainWindow:

```cpp
// In MainWindow constructor
led_(BabyMonitorConfig::LED_CHIP_NUMBER, BabyMonitorConfig::LED_PIN_NUMBER)
```

### 2. Trigger Blinking

Trigger LED blinking when motion anomaly is detected:


### 3. Configuration Parameters

Define LED-related configuration in `src/utils/Config.h`:




### Trigger Scenarios

LED blinking is triggered in the following situations:

1. **Motion Anomaly Alert**: When "no motion" state is detected 5 consecutive times
2. **System Status Indication**: Provides visual feedback to users

### Interaction with Other Modules

```
Motion Detection → MainWindow → LED Controller
      ↓               ↓              ↓
  No Motion       Trigger Alert    LED Blink
  Detection
```

## Technical Features

### Advantages

1. **Easy to Use**: Header-only design, no need to compile additional source files
2. **Thread Safe**: Prevents multiple blinking operations from running simultaneously
3. **Resource Safe**: RAII pattern ensures proper resource release
4. **High Precision**: Uses std::chrono for precise timing control
5. **Non-blocking**: Asynchronous execution doesn't affect main program operation

### Design Patterns

1. **RAII**: Resource Acquisition Is Initialization, ensures resource management
2. **Asynchronous Execution**: Uses std::thread for non-blocking operations
3. **Atomic Operations**: Uses std::atomic to ensure thread safety




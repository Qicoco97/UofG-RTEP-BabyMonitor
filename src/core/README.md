# Core Module - Architecture Foundation & Dependency Injection

## Overview

The Core module is the architectural foundation of the UofG-RTEP-BabyMonitor system, responsible for implementing Dependency Injection (DI) and application startup management. This module follows SOLID design principles, providing **loosely coupled, testable, and extensible** architectural support for the system.

## Core Components

### 1. ApplicationBootstrap (Application Bootstrap)

**Function**: Responsible for unified application startup process.

**Role**: Coordinates initialization of managers, ui, sensors and other components, and performs startup validation and error recovery.

### 2. ServiceContainer (Dependency Injection Container)

**Function**: Provides service registration and resolution mechanisms.

**Role**: Manages dependencies through interfaces rather than concrete classes, supporting both singleton and factory lifecycles.

## Interaction with Other Modules

### interfaces/
- Defines standard interfaces such as IComponent, IAlarmSystem, ISensorManager. The Core module manages cross-layer dependencies through these interfaces

### managers/
- For example, AlarmSystem implements the IAlarmSystem interface. Registered as an injectable service through Core's container, responsible for DDS communication and alarm logic

### ui/
- MainWindow uses setAlarmSystem() to receive injected alarm services. Achieves complete decoupling between UI and business logic

### utils/
- Integrates ErrorHandler to implement unified error handling for startup and runtime. Centralized system log management for debugging and maintenance

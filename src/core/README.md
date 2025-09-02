# Core Module - Architecture Foundation & Dependency Injection

## Overview

The `core` module is the architectural foundation of the Baby Monitor system, implementing Dependency Injection (DI) and application startup management. This module follows SOLID design principles, providing a loosely coupled, testable, and extensible architectural foundation.

## Module Components

The core module provides(Adventages):
- ⭐ **Application Bootstrap** - Unified application initialization process
- ⭐ **Dependency Injection Container** - Managing inter-component dependencies
- ⭐ **Service Lifecycle Management** - Controlling creation and destruction of system services
- ⭐ **Architecture Decoupling** - Implementing interface-driven loose coupling design

## Position in the New Architecture

```
src/
├── core/                    ← Core Architecture Layer
│   ├── ApplicationBootstrap  ← Application Startup Bootstrap
│   └── ServiceContainer     ← Dependency Injection Container
├── interfaces/              
├── managers/               ← (uses core injection)
├── ui/                     ← (receives core injection)
├── sensors/                
├── communication/          
├── hardware/              
└── utils/                 
```
## Integration with Architecture Layers

### ⭐ **interfaces/ Layer Integration**
- Defines standard interfaces: `IComponent`, `IAlarmSystem`, `ISensorManager`
- Core module provides type-safe service management through these interfaces

### ⭐ **managers/ Layer Integration**
- `AlarmSystem` implements `IAlarmSystem` interface
- Registered as injectable service through core container
- Manages DDS communication and alarm logic

### ⭐ **ui/ Layer Integration**
- `MainWindow` receives injected services through `setAlarmSystem()`
- Uses `injectedAlarmSystem_` for alarm publishing
- Achieves complete decoupling between UI and business logic

### ⭐ **utils/ Layer Integration**
- Integrates `ErrorHandler` for unified error handling
- All startup process logs are recorded through ErrorHandler

## Core Components Detailed

### 1. ApplicationBootstrap.h/cpp
**Application Startup Bootstrap**

#### Role in the New Architecture:
- ⭐ **Unified Startup Entry**: Provides unified startup process for the entire layered architecture
- ⭐ **Service Orchestration**: Coordinates initialization of managers, ui, sensors and other layer components
- ⭐ **Startup Validation**: Ensures all critical services are properly registered and configured
- ⭐ **Error Recovery**: Handles exceptions during startup, provides user-friendly error messages

### 2. ServiceContainer.h/cpp
**Dependency Injection Container**

#### Role in the New Architecture:
- ⭐ **Cross-Layer Service Management**: Manages service instances across managers, ui, sensors and other layers
- ⭐ **Smart Service Resolution**: Type-safely resolves concrete implementations based on interfaces
- ⭐ **Lifecycle Control**: Supports both singleton and factory service lifecycles
- ⭐ **Interface Decoupling**: Manages dependencies through interfaces rather than concrete classes

#### Supported Service Patterns:
- **Factory Services**: Creates new instances on each resolution (suitable for stateful services)
- **Singleton Services**: Globally shared instances (suitable for stateless utility classes)
- **Interface Binding**: Type-safe binding through `IComponent`, `IAlarmSystem` and other interfaces

## Design Patterns in the Refactored Architecture

### 1. Dependency Injection Pattern
**Solving Inter-Module Coupling Issues**
```cpp
// Register management layer services in main.cpp
container.registerService<BabyMonitor::IAlarmSystem>("AlarmSystem", []() {
    return std::make_shared<BabyMonitor::AlarmSystem>();
});

// Receive injected dependencies in UI layer
auto alarmSystem = container.resolve<BabyMonitor::IAlarmSystem>("AlarmSystem");
mainWindow.setAlarmSystem(alarmSystem);
```

### 2. Singleton Pattern
**Ensuring Global Container Uniqueness**
```cpp
ServiceContainer& ServiceContainer::getInstance() {
    static ServiceContainer instance;  // Thread-safe singleton
    return instance;
}
```

### 3. Interface Segregation Pattern
**Achieving Layer Decoupling Through Interfaces**
- `IAlarmSystem` - Alarm system interface
- `IComponent` - Base component interface
- `ISensorManager` - Sensor management interface


## Implementation Considerations

### Qt MOC Compatibility
- Due to Qt MOC (Meta-Object Compiler) limitations, MainWindow creation is deferred to `main.cpp`
- This avoids complexity of handling Qt objects directly in the core module

### Thread Safety
- ServiceContainer uses singleton pattern, safe in multi-threaded environments
- Service resolution operations are thread-safe

### Memory Management
- Uses `std::shared_ptr` for automatic memory management
- Prevents memory leaks and dangling pointer issues

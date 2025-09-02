# Managers Module

## 概述

Managers模块是UofG-RTEP-BabyMonitor项目的业务逻辑管理层，负责实现系统的核心管理功能。该模块通过实现interfaces模块定义的接口，为系统提供报警管理和传感器管理等关键服务，是连接底层组件和上层UI的重要桥梁。


## 核心组件

### 1. AlarmSystem - 报警系统管理器

实现`IAlarmSystem`接口，负责管理系统的报警发布和DDS通信功能。

#### 主要功能

- **报警发布**: 通过DDS协议发布报警消息到网络
- **消息格式化**: 根据严重程度格式化报警消息
- **性能监控**: 集成性能监控，支持自适应发布频率
- **生命周期管理**: 完整的初始化、启动、停止流程


#### 性能自适应

- **自适应发布频率**: 根据系统性能动态调整发布间隔
- **性能监控**: 记录每次发布的延迟时间
- **智能降级**: 在高负载时减少发布频率

### 2. SensorManager - 传感器管理器

实现`ISensorManager`接口，负责管理系统中的多个传感器组件。

#### 主要功能

- **传感器注册**: 动态添加和移除传感器
- **生命周期管理**: 统一管理所有传感器的启动和停止
- **健康监控**: 实时监控传感器状态
- **错误处理**: 识别和报告故障传感器

#### 信号机制

```cpp
signals:
    void sensorAdded(const QString& name);
    void sensorRemoved(const QString& name);
    void sensorStatusChanged(const QString& name, bool isHealthy);
    void allSensorsHealthy();
    void sensorsWithErrors(const QStringList& errorSensors);
```


## 依赖注入集成

通过ServiceContainer进行依赖注入：

```cpp
// 在main.cpp中注册服务
container.registerService<BabyMonitor::IAlarmSystem>("AlarmSystem", []() {
    return std::make_shared<BabyMonitor::AlarmSystem>();
});

// 在MainWindow中使用
auto alarmSystem = container.resolve<BabyMonitor::IAlarmSystem>("AlarmSystem");
mainWindow.setAlarmSystem(alarmSystem);
```

### 报警系统使用

```cpp
// 在MainWindow的timerEvent中
if (injectedAlarmSystem_) {
    if (!motionDetected_) {
        QString message = QString("No motion detected !!!! Dangerous!");
        injectedAlarmSystem_->publishAlarm(message, 3); // 高严重程度
    } else {
        QString message = QString("On motion !!!");
        injectedAlarmSystem_->publishAlarm(message, 1); // 低严重程度
    }
}
```

### 传感器管理使用

```cpp
// 添加传感器到管理器
sensorManager->addSensor("DHT11", dht11Worker);
sensorManager->addSensor("MotionDetector", motionWorker);

// 启动所有传感器
sensorManager->start();

// 检查传感器健康状态
if (!sensorManager->areAllSensorsHealthy()) {
    QStringList errorSensors = sensorManager->getErrorSensors();
    // 处理故障传感器...
}
```

## 架构优势

### 1. 接口驱动设计

- 实现标准化接口，确保组件间的松耦合
- 支持依赖注入，提高代码的可测试性
- 易于替换实现，增强系统灵活性

### 2. 统一管理

- **AlarmSystem**: 集中管理所有报警逻辑
- **SensorManager**: 统一管理传感器生命周期
- 简化上层调用，降低系统复杂度

### 3. 性能优化

- 集成性能监控系统
- 支持自适应性能调整
- 实时监控组件健康状态

### 4. 错误处理

- 完善的错误报告机制
- 通过ErrorHandler统一处理错误
- 支持优雅降级和恢复


## 与其他模块的交互

### 系统架构位置

```
UI Layer (MainWindow)
        ↓ (依赖注入)
Managers Layer (AlarmSystem, SensorManager)
        ↓ (接口调用)
Communication Layer (DDS) + Sensors Layer
```

### 数据流

```
传感器数据 → MainWindow → AlarmSystem → DDS网络
传感器组件 → SensorManager → 健康状态监控
```


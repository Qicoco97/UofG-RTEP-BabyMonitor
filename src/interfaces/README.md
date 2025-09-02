# Interfaces Module

## 概述

Interfaces模块定义了UofG-RTEP-BabyMonitor项目的核心接口，遵循接口隔离原则(Interface Segregation Principle)，为系统各组件提供标准化的抽象接口。该模块是实现依赖注入和松耦合架构的基础。


## 核心接口

### 1. IComponent - 基础组件接口

所有系统组件的基础接口，定义了组件的生命周期管理：

```cpp
class IComponent {
public:
    virtual bool initialize() = 0;      // 初始化组件
    virtual void start() = 0;           // 启动组件
    virtual void stop() = 0;            // 停止组件
    virtual bool isRunning() const = 0; // 检查运行状态
    virtual QString getName() const = 0; // 获取组件名称
    virtual bool isHealthy() const = 0;  // 检查健康状态
};
```

### 2. ISensorManager - 传感器管理接口

管理多个传感器的组接口：

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

### 3. IAlarmSystem - 报警系统接口

处理报警发布和管理的接口：

```cpp
class IAlarmSystem : public IComponent {
public:
    virtual bool publishAlarm(const QString& message, int severity = 1) = 0;
    virtual bool hasSubscribers() const = 0;
    virtual void setPublishInterval(int intervalMs) = 0;
};
```

### 4. IDisplayManager - 显示管理接口

处理UI更新和图表管理的接口：

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

## 接口实现

### 已实现的接口

1. **AlarmSystem** - 实现了`IAlarmSystem`接口
   - 位置: `src/managers/AlarmSystem.h/cpp`
   - 功能: DDS通信和报警逻辑管理

2. **SensorManager** - 实现了`ISensorManager`接口
   - 位置: `src/managers/SensorManager.h/cpp`
   - 功能: 传感器组管理和健康监控

## 在项目中的使用

### 依赖注入模式

通过接口实现依赖注入，降低组件间耦合：

```cpp
// MainWindow中使用接口
std::shared_ptr<BabyMonitor::IAlarmSystem> injectedAlarmSystem_;

// 通过依赖注入设置实现
void setAlarmSystem(std::shared_ptr<IAlarmSystem> alarmSystem) {
    injectedAlarmSystem_ = alarmSystem;
}

// 使用接口方法
if (injectedAlarmSystem_) {
    injectedAlarmSystem_->publishAlarm(message, severity);
}
```

### 服务容器集成

与核心模块的ServiceContainer配合使用：

```cpp
// 注册服务
container.registerService<IAlarmSystem>("AlarmSystem", 
    std::make_shared<AlarmSystem>());

// 解析服务
auto alarmSystem = container.resolve<IAlarmSystem>("AlarmSystem");
```

## 架构优势

### 1. 松耦合
- 组件间通过接口通信
- 易于替换实现
- 提高代码灵活性

### 2. 可测试性
- 易于创建Mock对象
- 支持单元测试
- 隔离测试环境

### 3. 可扩展性
- 新组件只需实现接口
- 不影响现有代码
- 支持插件式架构




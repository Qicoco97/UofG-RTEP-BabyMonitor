# Sensors Module

## 概述

Sensors模块是UofG-RTEP-BabyMonitor项目的传感器管理层，负责处理各种物理传感器的数据采集和管理。该模块提供温湿度监测、传感器工厂创建和统一的传感器管理接口，为婴儿监护系统提供环境监测能力。

## 文件结构

```
src/sensors/
├── DHT11Gpio.h         # DHT11传感器GPIO底层驱动
├── DHT11Gpio.cpp       # DHT11传感器GPIO底层实现
├── DHT11Worker.h       # DHT11传感器工作线程
├── DHT11Worker.cpp     # DHT11传感器工作线程实现
├── SensorFactory.h     # 传感器工厂类
└── CMakeLists.txt      # 构建配置文件
```

## 核心组件

### 1. DHT11Gpio - DHT11底层驱动

负责DHT11温湿度传感器的底层GPIO通信：

#### 主要功能

- **GPIO控制**: 使用libgpiod库直接操作GPIO引脚
- **协议实现**: 实现DHT11单总线通信协议
- **数据解析**: 解析温湿度数据和校验和
- **错误处理**: 处理通信超时和数据校验错误


### 2. DHT11Worker - DHT11工作线程

提供异步的温湿度数据采集服务：

#### 主要特性

- **多线程运行**: 在独立线程中执行，不阻塞主程序
- **定时采集**: 可配置的数据采集间隔
- **信号通知**: 通过Qt信号槽机制发送数据
- **错误恢复**: 自动处理读取错误和传感器故障


### 3. SensorFactory - 传感器工厂

遵循开闭原则的传感器创建工厂：


## 在项目中的使用

### 1. DHT11传感器集成

在MainWindow中使用DHT11传感器：


### 2. 数据处理

处理温湿度数据并更新UI：


### 3. 错误处理

处理传感器读取错误：


## 配置参数

在`src/utils/Config.h`中定义传感器相关配置：

```cpp
namespace BabyMonitorConfig {
    // GPIO配置
    constexpr int DHT11_PIN_NUMBER = 17;                    // DHT11引脚编号
    constexpr const char* GPIO_CHIP_DEVICE = "/dev/gpiochip0";  // GPIO设备路径
    
    // DHT11配置
    constexpr int DHT11_READ_INTERVAL_S = 3;                // 读取间隔（秒）
    constexpr int DHT11_MAX_CONSECUTIVE_ERRORS = 5;         // 最大连续错误次数
}
```

## 传感器管理

### 生命周期管理

传感器遵循统一的生命周期管理：

1. **初始化**: 通过SensorFactory创建传感器实例
2. **配置**: 连接信号槽和设置参数
3. **启动**: 开始数据采集线程
4. **运行**: 持续监控和数据上报
5. **停止**: 优雅关闭和资源清理

### 错误恢复机制

- **连续错误计数**: 跟踪连续失败次数
- **自动重试**: 传感器自动尝试重新读取
- **状态恢复**: 成功读取后重置错误计数
- **系统通知**: 通过ErrorHandler报告状态变化


## 架构优势

### 1. 模块化设计

- **分层架构**: 底层驱动、工作线程、工厂创建分离
- **职责清晰**: 每个组件专注特定功能
- **易于扩展**: 可轻松添加新的传感器类型

### 2. 异步处理

- **非阻塞**: 传感器读取在独立线程中进行
- **响应性**: 不影响主UI线程的响应性
- **并发性**: 多个传感器可同时工作

### 3. 错误容错

- **健壮性**: 完善的错误处理和恢复机制
- **稳定性**: 传感器故障不影响系统其他功能
- **监控性**: 实时监控传感器健康状态

### 4. 工厂模式

- **统一创建**: 通过工厂统一创建传感器实例
- **配置管理**: 集中管理传感器配置参数
- **内存管理**: 利用Qt父子关系自动管理内存

## 与其他模块的交互

### 系统架构位置

```
Hardware Sensors → DHT11Gpio → DHT11Worker → MainWindow → UI Display
                                    ↓              ↓
                              SensorFactory → Chart Updates
```

### 数据流

```
物理传感器 → GPIO读取 → 数据解析 → 信号发送 → UI更新
     ↓           ↓         ↓         ↓        ↓
   DHT11    DHT11Gpio  DHT11Worker  MainWindow  Charts
```

### 集成组件

- **MainWindow**: 接收传感器数据并更新UI
- **ErrorHandler**: 处理传感器错误和状态报告
- **Config**: 提供传感器配置参数
- **SensorManager**: 统一管理多个传感器（可选）





# LED Controller Module

## 概述

LED Controller模块是UofG-RTEP-BabyMonitor项目的硬件控制组件，负责控制LED指示灯的闪烁行为。该模块使用libgpiod库直接操作GPIO引脚，为婴儿监护系统提供视觉状态指示功能。


## 核心功能

### LEDController类

`LEDController`是一个header-only的C++类，提供简单而可靠的LED控制功能。

#### 主要特性

1. **GPIO控制**: 使用libgpiod库直接控制GPIO引脚
2. **异步闪烁**: 在后台线程中执行闪烁操作，不阻塞主程序
3. **资源管理**: 构造时打开GPIO，析构时自动释放资源
4. **线程安全**: 使用原子变量防止并发闪烁操作
5. **精确计时**: 使用高精度时间控制闪烁间隔

#### 核心方法

```cpp
// 构造函数 - 初始化GPIO
LEDController(int chipNo, int lineNo);

// 异步闪烁方法
void blink(int n = 3, int onMs = 200, int offMs = 100);
```

## 在项目中的使用

### 1. 初始化

在MainWindow中初始化LED控制器：

```cpp
// 在MainWindow构造函数中
led_(BabyMonitorConfig::LED_CHIP_NUMBER, BabyMonitorConfig::LED_PIN_NUMBER)
```

### 2. 触发闪烁

当检测到运动异常时触发LED闪烁：


### 3. 配置参数

在`src/utils/Config.h`中定义LED相关配置：




### 触发场景

LED闪烁在以下情况下被触发：

1. **运动异常警报**: 连续5次检测到"无运动"状态时
2. **系统状态指示**: 提供视觉反馈给用户

### 与其他模块的交互

```
Motion Detection → MainWindow → LED Controller
      ↓               ↓              ↓
   无运动检测      触发警报        LED闪烁
```

## 技术特点

### 优势

1. **简单易用**: Header-only设计，无需编译额外的源文件
2. **线程安全**: 防止多个闪烁操作同时进行
3. **资源安全**: RAII模式确保资源正确释放
4. **高精度**: 使用std::chrono实现精确的时间控制
5. **非阻塞**: 异步执行不影响主程序运行

### 设计模式

1. **RAII**: 资源获取即初始化，确保资源管理
2. **异步执行**: 使用std::thread实现非阻塞操作
3. **原子操作**: 使用std::atomic确保线程安全




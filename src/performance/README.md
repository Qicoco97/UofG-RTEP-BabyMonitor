# Performance Monitoring Module

## 概述

Performance模块是UofG-RTEP-BabyMonitor项目的性能监控系统，负责实时监控系统各组件的性能指标，确保婴儿监护系统满足实时性要求。该模块提供高精度计时、性能统计、约束检查和自适应性能调整等功能。

## 调试功能

### 键盘快捷键

- **P键**: 显示详细性能报告
- **A键**: 手动触发性能适配模式
- **R键**: 重置性能统计并恢复正常模式

### 性能显示

UI界面实时显示关键性能指标：
- 运动检测延迟和性能水平
- 帧处理延迟和性能水平
- 报警响应延迟和性能水平

## 性能监控指标

### 监控的操作类型

1. **MotionDetection** - 运动检测 (≤50ms)
2. **FrameProcessing** - 帧处理 (≤20ms)
3. **AlarmResponse** - 报警响应 (≤100ms)
4. **SensorReading** - 传感器读取 (≤300ms)
5. **UIUpdate** - UI更新 (≤30ms)

### 性能报告

系统自动生成详细的性能报告：

```
=== Performance Monitor Report ===
MotionWorker::MotionDetection:
  Average: 15.2ms
  Min: 8.1ms, Max: 45.3ms
  Samples: 150
  Status: OK (30.4%)

MainWindow::FrameProcessing:
  Average: 12.8ms
  Min: 5.2ms, Max: 28.1ms
  Samples: 200
  Status: OK (64.0%)
```

## 核心组件

### 1. RealTimeConstraints - 实时约束定义

定义了系统各操作的性能约束阈值：

```cpp
namespace RealTimeConstraints {
    constexpr double MAX_MOTION_DETECTION_LATENCY_MS = 50.0;     // 运动检测最大延迟
    constexpr double MAX_ALARM_RESPONSE_LATENCY_MS = 100.0;      // 报警响应最大延迟
    constexpr double MAX_FRAME_PROCESSING_LATENCY_MS = 20.0;     // 帧处理最大延迟
    constexpr double MAX_SENSOR_READ_LATENCY_MS = 300.0;         // 传感器读取最大延迟
    constexpr double MAX_UI_UPDATE_LATENCY_MS = 30.0;            // UI更新最大延迟
    
    // 性能自适应阈值
    constexpr double PERFORMANCE_ADAPTATION_THRESHOLD = 0.6;     // 60%时触发性能降级
    constexpr double PERFORMANCE_RECOVERY_THRESHOLD = 0.4;       // 40%时恢复正常模式
}
```

### 2. HighPrecisionTimer - 高精度计时器

提供微秒级精度的性能计时功能：

```cpp
class HighPrecisionTimer {
public:
    void start();                    // 开始计时
    double elapsedMs() const;        // 获取经过的毫秒数
    double elapsedUs() const;        // 获取经过的微秒数
};
```

### 3. PerformanceStats - 性能统计

收集和分析性能数据：

```cpp
class PerformanceStats {
public:
    void addSample(double value);           // 添加性能样本
    double getAverage() const;              // 获取平均值
    double getMin() const;                  // 获取最小值
    double getMax() const;                  // 获取最大值
    double getStdDev() const;               // 获取标准差
    double getPercentile(double p) const;   // 获取百分位数
};
```

### 4. PerformanceMonitor - 性能监控器

系统核心性能监控组件（单例模式）：

```cpp
class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance();
    // 记录操作延迟
    void recordLatency(const QString& component, const QString& operation, double latencyMs);
    // 检查是否需要性能适配
    bool shouldAdaptPerformance(const QString& component, const QString& operation);
    // 检查是否可以恢复性能
    bool canRecoverPerformance(const QString& component, const QString& operation);
    // 获取性能统计
    const PerformanceStats* getStats(const QString& component, const QString& operation) const;
    // 生成性能报告
    QString generatePerformanceReport() const;
};
```

## 在项目中的使用

### 1. 运动检测性能监控

在MotionWorker中监控帧处理性能：

### 2. 主窗口帧处理监控

在MainWindow中监控帧处理和报警响应：

### 3. 报警系统性能监控

在AlarmSystem中监控DDS发布性能：


## 性能自适应机制

### 自适应触发条件

- **性能降级**: 当平均延迟超过限制的60%时触发
- **性能恢复**: 当平均延迟降至限制的40%以下时恢复

### 自适应策略

1. **运动检测自适应**:
   - 减小高斯模糊核大小（21x21 → 15x15）
   - 提高二值化阈值
   - 增加最小面积阈值

2. **帧处理自适应**:
   - 启用帧跳跃机制
   - 降低处理频率

3. **报警发布自适应**:
   - 调整发布频率
   - 优化消息格式


## 架构优势

### 1. 实时监控

- 微秒级精度计时
- 实时约束检查
- 自动性能违规报告

### 2. 自适应优化

- 智能性能降级
- 自动质量恢复
- 动态参数调整

### 3. 统计分析

- 完整的性能统计
- 百分位数分析
- 趋势监控

### 4. 易于集成

- Header-only设计
- 单例模式访问
- 最小化性能开销


## 与其他模块的交互

### 系统架构位置

```
所有组件 → PerformanceMonitor → 性能统计和自适应
    ↓              ↓                    ↓
计时测量        约束检查            性能调整
```

### 集成组件

- **MotionWorker**: 运动检测性能监控
- **MainWindow**: 帧处理和UI性能监控
- **AlarmSystem**: 报警发布性能监控
- **ErrorHandler**: 性能违规报告

---


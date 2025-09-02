# Motion Detection Module

## 概述

Detection模块是UofG-RTEP-BabyMonitor项目的核心组件之一，负责实时视频流中的运动检测功能。该模块采用基于OpenCV的计算机视觉算法，为婴儿监护系统提供智能运动感知能力。

## 文件结构

```
src/detection/
├── motionworker.h      # MotionWorker类声明文件
└── motionworker.cpp    # MotionWorker类实现文件
```

## 核心组件

### MotionWorker类

`MotionWorker`是一个继承自`QObject`的工作类，专门设计用于在独立线程中执行运动检测任务，避免阻塞主UI线程。

#### 主要特性

1. **多线程处理**: 运行在独立的QThread中，确保UI响应性
2. **自适应性能优化**: 根据系统性能动态调整检测参数
3. **实时性能监控**: 集成高精度计时器和性能监控系统
4. **信号槽机制**: 通过Qt信号槽与其他组件通信

#### 核心算法

运动检测采用经典的帧差法（Frame Difference）算法：

1. **预处理**: 将彩色帧转换为灰度图像
2. **高斯模糊**: 使用可调节的核大小进行噪声过滤
3. **帧差计算**: 计算当前帧与前一帧的绝对差值
4. **二值化**: 应用阈值将差值图像转换为二值图像
5. **形态学操作**: 使用膨胀操作连接相邻的运动区域
6. **轮廓检测**: 查找并分析运动区域的轮廓
7. **面积过滤**: 根据最小面积阈值过滤小的噪声区域

#### 性能自适应机制

系统具备智能的性能自适应能力：

**性能降级模式**（当检测到性能瓶颈时）：
- 减小高斯模糊核大小（21x21 → 15x15）
- 提高二值化阈值（thresh + 10）
- 增加最小面积阈值（minArea × 1.5）

**性能恢复模式**（当系统性能改善时）：
- 恢复原始参数设置
- 提供最佳检测质量

## 在项目中的集成

### 1. 系统架构位置

```
Camera (libcam2opencv) → MainWindow → MotionWorker → AlarmSystem
                                   ↓
                              Performance Monitor
```

### 2. 创建和初始化

通过`SensorFactory`工厂类创建：


### 3. 信号槽连接

```cpp
// 帧处理连接
connect(frameSource, SIGNAL(frameReady(const cv::Mat&)), 
        motionWorker_, SLOT(processFrame(const cv::Mat&)));

// 运动检测结果连接
connect(motionWorker_, SIGNAL(motionDetected(bool)),
        mainWindow, SLOT(onMotionStatusChanged(bool)));

// 性能警报连接
connect(motionWorker_, &MotionWorker::performanceAlert,
        mainWindow, &MainWindow::onMotionWorkerPerformanceAlert);
```

### 4. 配置参数

在`src/utils/Config.h`中定义：

```cpp
constexpr double MOTION_MIN_AREA = 500.0;    // 最小运动区域面积
constexpr int MOTION_THRESHOLD = 25;         // 二值化阈值
```

## 性能监控集成

### 实时性能指标

- **处理延迟**: 每帧处理时间监控
- **性能阈值**: 最大运动检测延迟 ≤ 50ms
- **自适应触发**: 当平均延迟超过阈值的60%时触发性能降级
- **恢复条件**: 当延迟降至阈值的40%以下时恢复正常模式

### 性能监控API

```cpp
// 记录处理时间
perfMonitor_->recordLatency("MotionWorker", "MotionDetection", processingTime);
// 检查是否需要性能适配
if (perfMonitor_->shouldAdaptPerformance("MotionWorker", "MotionDetection")) {
    adaptForPerformance();
}
```

## 与其他模块的交互

### 1. 摄像头模块 (Camera)
- **输入**: 接收来自`libcam2opencv`的实时视频帧
- **格式**: `cv::Mat`格式的BGR图像

### 2. 用户界面 (UI)
- **输出**: 运动检测状态更新UI标签
- **显示**: "On motion" / "No motion"状态

### 3. 报警系统 (AlarmSystem)
- **触发**: 运动检测结果用于触发报警逻辑
- **阈值**: 连续5次"无运动"检测后触发报警

### 4. LED控制器
- **指示**: 运动检测时控制LED闪烁
- **视觉反馈**: 为用户提供直观的状态指示

### 5. DDS通信
- **发布**: 运动检测结果通过DDS发布到网络
- **数据结构**: `BabyMonitor::MotionData`



## 技术特点

### 优势

1. **高性能**: 优化的OpenCV算法实现
2. **自适应**: 根据系统负载动态调整参数
3. **实时性**: 满足婴儿监护的实时性要求
4. **可扩展**: 模块化设计便于功能扩展
5. **可测试**: 提供测试接口和调试功能


## 依赖关系

### 外部依赖
- **OpenCV**: 计算机视觉算法库
- **Qt**: 多线程和信号槽机制

### 内部依赖
- **PerformanceMonitor**: 性能监控系统
- **ErrorHandler**: 错误处理系统
- **Config**: 配置管理系统




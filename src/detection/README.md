# Detection Module

## Overview

The Detection module is one of the core components of the UofG-RTEP-BabyMonitor project, responsible for motion detection functionality in real-time video streams. This module uses OpenCV-based computer vision algorithms to provide intelligent motion sensing capabilities for the baby monitoring system.

## Core Components

### 1. MotionWorker - Motion Detection Worker

A worker class inheriting from `QObject`, specifically designed to execute motion detection tasks in an independent thread:

**Main Functions**:
- Runs in an independent QThread to ensure UI responsiveness
- Dynamically adjusts detection parameters based on system performance
- Integrates high-precision timers and performance monitoring systems
- Communicates with other components through Qt signal-slot mechanism

**Core Algorithms**:
- Preprocessing: Convert color frames to grayscale images
- Gaussian blur: Use adjustable kernel size for noise filtering
- Frame difference calculation: Calculate absolute difference between current and previous frames
- Binarization: Apply threshold to convert difference image to binary image
- Morphological operations: Use dilation to connect adjacent motion regions
- Contour detection: Find and analyze contours of motion regions
- Area filtering: Filter small noise regions based on minimum area threshold

**Performance Adaptation Mechanism**:
- Reduce Gaussian blur kernel size, increase binarization threshold, increase minimum area threshold
- Restore original parameter settings to provide optimal detection quality

## Interaction with Other Modules

### ui/
- Sends motion detection results to `MainWindow` to update UI status display
- Implements real-time status updates through signal-slot mechanism ("On motion" / "No motion")

### managers/
- Motion detection results are used to trigger `AlarmSystem` alarm logic
- Triggers alarm publishing after 5 consecutive "no motion" detections

### hardware/
- Controls LED indicators to blink when motion is detected
- Provides intuitive visual status feedback for users

### camera/
- Receives real-time video frames from `libcam2opencv`
- Processes BGR image data in `cv::Mat` format

### performance/
- Integrates `PerformanceMonitor` for real-time performance monitoring
- Records processing latency and triggers adaptive performance adjustments






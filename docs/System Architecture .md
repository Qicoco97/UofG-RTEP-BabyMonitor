
#### ** Control Flow Sequence**

```
1. System Initialization
   ApplicationBootstrap → ServiceContainer → Component Registration

2. Sensor Activation
   SensorFactory → DHT11Worker → GPIO Initialization → Data Collection

3. Camera Processing
   Libcam2OpenCV → Frame Capture → MotionWorker → Analysis Results

4. Data Integration
   MainWindow ← Sensor Data ← Motion Results ← Performance Metrics

5. Alert Processing
   Motion Status → AlarmSystem → DDS Publishing → LED/Audio Alerts

6. Performance Monitoring
   PerformanceMonitor → Adaptive Adjustment → System Optimization

7. Error Handling
   ErrorHandler ← All Components ← Status Reports ← Recovery Actions
```

#### ** Real-Time Processing Pipeline**

| **Stage** | **Component** | **Processing Time** | **Output** |
|-----------|---------------|-------------------|------------|
| **Frame Capture** | Libcam2OpenCV | ~16ms (30 FPS) | Raw video frames |
| **Motion Detection** | MotionWorker | <50ms | Motion status |
| **Sensor Reading** | DHT11Worker | ~300ms | Temp/Humidity |
| **UI Update** | MainWindow | <30ms | Visual display |
| **Alert Publishing** | AlarmSystem | <100ms | Network messages |
| **Performance Check** | PerformanceMonitor | ~5ms | System metrics |

# Baby Monitor: Real‑Time Embedded Sensor & Monitoring Dashboard

This is our **smart baby monitoring system**, designed to provide comprehensive real-time monitoring for infant safety and environmental conditions. The system combines motion detection, temperature/humidity monitoring, and intelligent alerting to help parents ensure their baby's wellbeing. With integrated camera monitoring, environmental sensors, and network communication capabilities, this platform delivers a complete monitoring solution that can be extended with additional sensors as needed.

---

## 📊 System Overview

### 🎥 **Real-Time Video Monitoring & Motion Analysis**

Seamless camera integration via `libcam2opencv` enables continuous video capture with real-time frame processing. Intelligent motion detection, powered by OpenCV frame-difference algorithms, identifies baby movement patterns. The Qt-based GUI provides live video feedback with clear motion status indicators (“On motion” / “No motion”).
<p align="center">
  <img src="./img%26demo/mainwindow.png" alt="mainwindow">
</p>

### 🌡️ **Environmental Monitoring**

Real-time `temperature and humidity monitoring` is enabled by the DHT11 sensor with 3-second update intervals. Data is displayed through dynamic trend charts with configurable data point limits, providing clear visualization of environmental changes.


### 🚨 **Multi-Channel Alert System**

When the system detects "no motion" for five consecutive times(After conducting tests for several times, it was determined that five times would be a suitable duration.), it determines that the monitored object is in danger. At this point, the `LED lights` will flash and the `speaker` will play the alarm audio. The system supports real-time data exchange through `DDS`, enabling seamless dissemination of environmental and motion data to subscriber terminals.
- Here is the feedback information from the subscriber terminal.
<p align="center">
  <img src="./img%26demo/Subscribe.png" alt="Subscribe" height="300">
</p>

- Here is the feedback information from the publisher terminal.
<p align="center">
  <img src="./img%26demo/publisher.png" alt="publisher" height="300">
</p>

### 🧠 **Intelligent Performance Management**

The system ensures adaptive processing with dynamic parameter tuning based on load conditions, maintaining real-time responsiveness with motion detection latency under 50 ms. Resource optimization through adaptive quality scaling balances performance and efficiency, while robust error recovery mechanisms provide automatic sensor restoration and continuous health monitoring.
<p align="center">
  <img src="./img%26demo/performancewindow.png" alt="performancewindow">
</p>

---


##  Demo Preview

- [System Demo Video](https://github.com/Qicoco97/UofG-RTEP-BabyMonitor/blob/master/img%26demo/DEMO.mp4)
- Also you can see the project at our social media.
links: https://www.tiktok.com/@gavinzet0?_t=ZN-8zOjntmHSsw&_r=1

---


## 🏗️ System Architecture

<p align="center">
  <img src="./img%26demo/Architecture.png" alt="Subscribe" height="600">
</p>

### **Module Division & Documentation**

Our system consists of **11 different modules**, each performing its own specific task. You can click on the corresponding links to view the specific methods and functions they contain.

#### **Core Module** - *System Foundation*
- **[Core Module](./src/core/README.md)** 
Provides dependency injection and application bootstrap management, serving as the architectural foundationcontainer
#### **Interface Module** - *Contract Definitions*
- **[Interfaces Module](./src/interfaces/README.md)** - Defines standardized interfaces (e.g., for alarms, sensors, display management) to support loose coupling and dependency injection
#### **Management Module** - *Business Logic Controllers*
- **[Managers Module](./src/managers/README.md)** - Implements alarm and sensor management, handling business logic and interactions with lower-level components
#### **UI Module** - *User Interface & Interaction*
- **[UI Module](./src/ui/README.md)** - Qt-based main interface integrating video display, sensor data, motion detection, alarms, and visualization
#### **Detection Module** - *Computer Vision & Analysis*
- **[Detection Module](./src/detection/README.md)** - Performs motion detection using OpenCV’s frame-difference method, with multithreading and adaptive performance optimization
#### **Sensor Module** - *Environmental Monitoring*
- **[Sensors Module](./src/sensors/README.md)** - Integrates DHT11 and other sensors, supporting data acquisition and sensor lifecycle management
#### **Camera Module** - *Environmental Monitoring*
- **[camera Module](./src/camera/README.md)** - Integrates libcamera for video capture and processing, using the libcamera2opencv library from Professor Bernd Porr's GitHub repository.
#### **Hardware Module** - *Physical Device Control*
- **[Hardware Module](./src/hardware/README.md)** - Provides GPIO hardware control, including LED indicators and camera integration
#### **Communication Module** - *Network & Data Exchange*
- **[Communication Module](./src/communication/dds/README.md)** - Implements real-time message publishing and subscribing with Fast DDS, enabling alarm and system information transmission, using the Fast DDS library from Professor Bernd Porr's GitHub repository.
#### **Utility Module** - *Common Services & Tools*
- **[Utils Module](./src/utils/README.md)** - Offers centralized configuration management, unified error handling, and standardized data structures
####  **Performance Module** - *System Optimization*
- **[Performance Module](./src/performance/README.md)** - Monitors system performance metrics with real-time constraint checking and adaptive optimization

###  🎯 **Design Principles & Architecture Benefits**
- The system follows SOLID principles, with each module focused on a single responsibility and extended through minimal, well-defined interfaces. Dependency injection ensures loose coupling, allowing components to be interchangeable and the architecture to remain modular and maintainable.

- This layered design provides key advantages: modularity for easier testing and reuse, performance through multi-threading and adaptive optimization, and robustness with error isolation and self-recovery.

- Compared to monolithic systems, this architecture avoids tight coupling and single points of failure. Through separation of concerns, service-oriented design, and integrated monitoring, it ensures the baby monitoring system is reliable, scalable, and future-proof while maintaining the real-time performance essential for infant safety.

## 📕 Documentation (`/docs`)  
Here are also some detailed information about the system.

| File | Description |
|------|-------------|
| [`System Architecture.md`](https://github.com/Qicoco97/UofG-RTEP-BabyMonitor/blob/master/docs/System%20Architecture%20.md) | System architecture: hardware/software modules and interaction relationships |
| [`Analysis & Future Prospects.md`](https://github.com/Qicoco97/UofG-RTEP-BabyMonitor/blob/master/docs/Analysis%20%26%20Future%20Prospects.md) | Limitations analysis and future improvement directions |



---
## 🛠️ Hardware Setup
---
### 1. Raspberry Pi (Model 4/5 with 40‑pin GPIO header)

### 2. Sensors

- **DHT11 Temperature/Humidity Sensor (GPIO 17):** Monitors environmental conditions in real-time, enabling the system to alert parents when room conditions become unsuitable for infant comfort and health.
<p align="center">
  <img src="./img%26demo/DHT11.jpg" alt="DHT11" height="300">
</p>

- **Camera Module (libcamera interface):** Captures continuous video stream for motion detection analysis and provides live visual monitoring of the baby's area.
<p align="center">
  <img src="./img%26demo/camera.jpg" alt="camera" height="300">
</p>

### 3. Visual Indicators & Alarm Device

- **LED Controller (GPIO 27):** Provides immediate visual feedback through configurable blinking patterns to indicate system status and motion detection events.
- **Speaker (HDMI):** Since the Raspberry Pi 5 we are using does not have a sound output interface, we used HDMI as the sound output. We also tested using a Bluetooth speaker as the sound output source.
<p align="center">
  <img src="./img%26demo/LEDlight.jpg" alt="LEDlight" height="300">
</p>

#### Wiring
For the wiring and GPIO use see the following pic
<p align="center">
  <img src="./img%26demo/Wiring.png" alt="Wiring" height="300">
</p>

## 💻 Software Build & Installation

#### Dependencies
``` bash
sudo apt update
sudo apt install libopencv-dev libgpiod-dev libwebsocketpp-dev libboost-all-dev libcamera-dev libfastcdr-dev libfastrtps-dev fastddsgen fastdds-tools
libqwt-qt5-dev qtdeclarative5-dev qtcharts
```

#### Build & Run
``` bash
git clone https://github.com/Qicoco97/UofG-RTEP-BabyMonitor.git
cd src
cmake .
make
./baby
```
if you want to subscribe the information, you need to following bash
``` bash
cd communication/dds
./DDSAlarmSubscriber
```



## 🔧 Tests Preview

| Test Module              | Description                            | Preview |
|---------------------------|----------------------------------------|---------|
| **LED Test**              | `tests/test_led/` LED control demo     | [LED Test](https://github.com/Qicoco97/UofG-RTEP-BabyMonitor/blob/master/img%26demo/LED%20Test.mp4) |
| **Pub-Sub Test**          | `tests/test_sub_pub/` DDS communication demo | [Sub-Pub Test](https://github.com/Qicoco97/UofG-RTEP-BabyMonitor/blob/master/img%26demo/pub-sub_test.png) |


---

##  Third-Party Libraries

- **Project Name**: libcamera2opencv
  - **Author**: Bernd Porr
  - **Link**: [https://github.com/berndporr/libcamera2opencv](https://github.com/berndporr/libcamera2opencv)
  - **License**: GPL-2.0 License

- **Project Name**: fastdds_demo
  - **Author**: Bernd Porr
  - **Link**: [https://github.com/berndporr/fastdds_demo](https://github.com/berndporr/fastdds_demo)
  - **License**:  Apache License 2.0

Thanks to my teacher and we learned a lot from this lecture !!! And the Original idea was came up with cribsense project! Thanks all
---

##  Important Notice (Resubmission)

Due to improper use of third-party libraries in the initial version, the project was deemed academic misconduct.
In this resubmission, we have completely removed and discontinued the prior external libraries while retaining the original project focus — a baby monitor — and, with reference to code provided by Dr. Bernd Porr, we have re-implemented and refactored the core functionality from scratch.
This decision both avoids controversy and dependency risks and modernizes the previously outdated technology stack, thereby enhancing traceability, compliance, and system stability.

---
##  License

This project is licensed under the MIT License. See `LICENSE` for details.

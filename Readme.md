# BabyMonitor  

This project is a real-time embedded system that monitors the environment and recognizes target behaviors to detect potential risks for infants and raise alerts.  
The system is designed to help parents better check their baby’s status, making it suitable for home applications.  

---

##  System Overview  

1. **Real-time Video Monitoring**  
   - The camera captures video (`libcam2opencv`) → The GUI displays frames in real-time (`mainwindow::updateImage`).  
   - Parents can directly observe the baby’s condition on the interface.  

2. **Motion Detection (Detecting Baby’s Activity or Abnormality)**  
   - The `motionworker` module compares consecutive frames → Determines whether there is motion.  
   - When motion is detected:  
     - GUI displays “On motion”  
     - LED blinks as a physical indicator  
     - The system publishes a motion message (via DDS to other systems/mobile devices)  
   - When no motion is detected for a long time:  
     - A danger alert is published (e.g., the baby is not moving, or the camera is blocked).  

3. **Environmental Monitoring (Temperature & Humidity)**  
   - `DHT11Worker` periodically reads temperature and humidity values.  
   - GUI displays the values and plots them on charts.  
   - Parents can determine whether the room is too hot/cold/humid and adjust air conditioning or humidifiers accordingly.  

4. **Alerts & Information Publishing (DDS Communication)**  
   - `AlarmPublisher` periodically checks motion status:  
     - No motion → Publish alert message `"No motion detected !!!! Dangerous!"`  
     - Motion detected → Publish `"On motion !!!"`  
   - These messages can be subscribed to by other devices, e.g.:  
     - Mobile app receives notifications  
     - IoT alarm device rings  
     - Data is uploaded to the cloud  

5. **Local Feedback (LED Indicator)**  
   - When motion is detected, the LED blinks as an immediate physical reminder.  

---

## Project Background & Motivation  

In modern households, infant safety and health are top concerns for parents. Traditional monitoring methods usually rely on manual supervision or simple video cameras, which face several limitations:  

- **Parental fatigue**: Continuous monitoring is exhausting and prone to lapses.  
- **Limited monitoring**: Standard cameras only provide visuals without intelligent analysis or timely alerts.  
- **Environmental neglect**: Babies are sensitive to temperature, humidity, and air quality—factors ignored by conventional devices.  
- **Lack of alerts & integration**: Existing devices rarely provide proactive alerts or integrate with other systems, leading to delayed notifications.  

To address these issues, we designed and implemented an **intelligent baby monitor system** with the following features:  

- **Real-time video monitoring**: Camera integration allows parents to check the baby’s activity in real-time.  
- **Smart motion detection**: Frame-difference algorithms detect movement; prolonged inactivity triggers alerts.  
- **Environmental monitoring**: Integrated DHT11 sensor continuously tracks room temperature and humidity, with chart visualization.  
- **Multi-channel alerts & feedback**:  
  - Alerts pushed to other devices (e.g., mobile apps or IoT alarms) via DDS communication.  
  - Local LED indicator provides immediate, visible reminders.  

---

##  Demo Preview  

- [System Demo Video](https://github.com/Qicoco97/UofG-RTEP-BabyMonitor/blob/master/img%26demo/DEMO.mp4)  

---

##  Tests Preview  

| Test Module              | Description                            | Preview |
|---------------------------|----------------------------------------|---------|
| **LED Test**              | `tests/test_led/` LED control demo     | [LED Test](https://github.com/Qicoco97/UofG-RTEP-BabyMonitor/blob/master/img%26demo/LED%20Test.mp4) |
| **Pub-Sub Test**          | `tests/test_sub_pub/` DDS communication demo | [Sub-Pub Test](https://github.com/Qicoco97/UofG-RTEP-BabyMonitor/blob/master/img%26demo/pub-sub_test.png) |

---

##  System Modules  

- **Camera Module**: Real-time video capture with libcamera & OpenCV  
- **Motion Detection**: Frame-difference based motion recognition (OpenCV)  
- **Environment Monitoring**: DHT11 sensor for temperature & humidity tracking  
- **Alarm & Communication**: DDS-based message publishing for alerts  
- **LED Feedback**: GPIO-controlled LED blinking on motion detection  
- **Qt GUI**: Live video stream, motion status, and environmental charts  

[Architecture Diagram](https://github.com/Qicoco97/UofG-RTEP-BabyMonitor/blob/master/img%26demo/Architecture%20Diagram.png)  
[Class](https://github.com/Qicoco97/UofG-RTEP-BabyMonitor/blob/master/img%26demo/Class.png)
[Data Flow](https://github.com/Qicoco97/UofG-RTEP-BabyMonitor/blob/master/img%26demo/Data%20Flow.png)

---

##  How to Run  

### 2.1 Build & Run  
```bash
mkdir build && cd build
cmake ..
make
./baby    # main program

---
```
###  2.2 Test Output  

- Qt GUI launches and displays real-time camera feed  
- `MotionWorker` detects motion and updates status in the GUI  
- DHT11 sensor data (temperature & humidity) displayed and plotted  
- If no motion for a long time → System sends `"No motion detected"` alert  
- If motion detected → System sends `"On motion"` alert and LED blinks  

---

##  Documentation (`/docs`)  


| File | Description |
|------|-------------|
| [`System Architecture.md`](https://github.com/Qicoco97/UofG-RTEP-BabyMonitor/blob/master/docs/System%20Architecture%20.md) | System architecture: hardware/software modules and interaction relationships |
| [`Analysis & Future Prospects.md`](https://github.com/Qicoco97/UofG-RTEP-BabyMonitor/blob/master/docs/Analysis%20%26%20Future%20Prospects.md) | Limitations analysis and future improvement directions |



---

##  Repository Structure  

- **img&demo/** — Demo images, wiring diagrams, and video demo  
  - DEMO.mp4 — System demo video  
  - LED.png — LED control diagram  
  - Output.png — Program output example  
  - Subscribe.png — DDS subscription test screenshot  
  - Wiring.png — Hardware wiring diagram  
  - dht11.png — DHT11 sensor module diagram  
  - diagram.png — System architecture/flow diagram  
  - pub-sub_test.png — DDS pub-sub test result  
  - window.png — Qt GUI screenshot  

- **src/** — Main source code of BabyMonitor project  
  - CMakeLists.txt — CMake build configuration  
  - LedController.h — GPIO LED control  
  - libcam2opencv.cpp, libcam2opencv.h — Camera capture & OpenCV bridge  
  - main.cpp — Program entry point  
  - mainwindow.cpp, mainwindow.h, mainwindow.ui — Qt main window & GUI logic  
  - motionworker.cpp, motionworker.h — Motion detection worker  
  - helloworld — Example test file  

- **tests/** — Unit test or module test folder (currently empty or placeholder)  
- **.gitignore** — Git exclusion rules (build folders, binaries, temp files)  
- **Readme.md** — Project overview, usage instructions, and documentation links  

---

##  Third-Party Libraries  

- **Project Name**: libcamera2opencv  
- **Author**: Bernd Porr  
- **Link**: [https://github.com/berndporr/libcamera2opencv](https://github.com/berndporr/libcamera2opencv)  
- **License**: GPL-2.0 License  

---

##  Important Notice (Resubmission)  

Due to improper use of third-party libraries in the initial version, the project was deemed academic misconduct.  
In this resubmission, we have completely removed and discontinued the prior external libraries while retaining the original project focus — a baby monitor — and, with reference to code provided by Dr. Bernd Porr, we have re-implemented and refactored the core functionality from scratch.  
This decision both avoids controversy and dependency risks and modernizes the previously outdated technology stack, thereby enhancing traceability, compliance, and system stability.  

---

##  License  

This project is licensed under the MIT License. See `LICENSE` for details.  

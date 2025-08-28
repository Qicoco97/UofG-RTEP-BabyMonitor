## System Architecture  

### 1. Overall Architecture Overview  
The system architecture of **BabyMonitor** consists of the following modules:  

- **Graphical User Interface (GUI)**  
  - Implemented using Qt Widgets + Charts  
  - Displays camera feed, environmental data curves, and motion status  
  - Provides visualization of LED control and alarm states  

- **Image Capture Module (Camera)**  
  - Uses `libcamera` to drive the camera  
  - Captured frames are converted to OpenCV `cv::Mat` via `libcam2opencv`  
  - Real-time frames are passed to the upper layer using a callback mechanism  

- **Motion Detection Module (MotionWorker)**  
  - Based on frame-difference and contour analysis  
  - Determines whether a motion target exists  
  - Sends detection results to the GUI  

- **Environmental Monitoring Module (DHT11)**  
  - Reads temperature and humidity data  
  - Updates data in real-time on the GUI  
  - Plots historical curves  

- **Alarm & Communication Module (DDS)**  
  - Uses FastDDS (`fastrtps + fastcdr`) to publish messages  
  - Sends corresponding alarm messages when motion/no-motion is detected  
  - Can be extended to notify remote devices or mobile apps  

- **Local Feedback Module (LED)**  
  - GPIO-controlled LED light  
  - Blinks when motion is detected as an intuitive physical indicator  

---

### 2. Module Interaction Relationships  

- **MainWindow (GUI Controller)**  
  - Calls `libcam2opencv` to obtain video frames → displays them in the interface  
  - Passes frames to `MotionWorker` → obtains motion detection results  
  - Retrieves temperature & humidity from `DHT11Worker` → plots data on chart  
  - Based on results from `MotionWorker`:  
    - Updates status label (`On motion / No motion`)  
    - Controls LED blinking  
    - Publishes alarm messages via `AlarmPublisher (DDS)`  

- **Camera → MotionWorker**  
  - Camera frames are input → motion detection → results fed back to GUI  

- **DHT11 → GUI**  
  - Temperature & humidity data → updated in text and charts  

- **MotionWorker → LED**  
  - Motion detected → triggers LED blinking  

- **MotionWorker/Timer → DDS**  
  - Periodically checks motion status → publishes alarm messages  

---

### 3. Architecture Diagram  

[Architecture Diagram](https://github.com/Qicoco97/UofG-RTEP-BabyMonitor/blob/master/img%26demo/Architecture%20Diagram.png)  

---

### 4. Features  

- **Modular Design**: Camera, motion detection, sensor, and communication are independent modules for easy expansion.  
- **Multithreading Mechanism**: Camera capture and motion detection run in separate threads, ensuring smooth GUI performance.  
- **High Extensibility**:  
  - Sensors can be replaced (e.g., DHT22, BME280)  
  - New alerting methods (e.g., SMS / mobile push notifications) can be added  
  - Cloud integration can be implemented for remote monitoring  

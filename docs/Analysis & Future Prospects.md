## Analysis & Future Prospects  

### 1. Limitations  

**Hardware Limitations**  
- The DHT11 sensor has low accuracy and limited sampling frequency, unsuitable for high-precision monitoring.  


**Software Limitations**  
- Motion detection algorithm is based on frame-difference and thresholding, prone to false positives under lighting changes or background noise.  
- DDS communication only supports basic publishing without encryption or security features, restricting real-world applications.  
- GUI functionality is limited, only providing data display and status indication, lacking remote interaction and mobile support.  

**System Integration Issues**  
- No full integration with cloud platforms or mobile apps; alerts cannot be pushed to actual user terminals.  
- No long-term stability testing; system reliability and power consumption require further optimization.  
- Prototype nature limits scalability, mass deployment, and maintainability.  

---

### 2. Future Prospects  

**Hardware Upgrades**  
- Replace with higher-precision sensors (e.g., DHT22, BME280) to improve environmental monitoring.  
- Add sound detection (e.g., crying recognition) and infrared sensors for multi-dimensional sensing.  
- Integrate buzzers or small speakers for more intuitive local alerts.  

**Algorithm Optimization**  
- Introduce machine learning / deep learning models to enhance accuracy and robustness of motion detection.  
- Add face recognition or posture recognition to differentiate baby’s actions from background noise.  
- Perform sensor fusion and anomaly detection to improve reliability of alerts.  

**Software & System Extensions**  
- Develop mobile apps to support remote real-time monitoring and notifications.  
- Connect to cloud platforms for data storage and long-term trend analysis.  
- Allow user-defined rules (e.g., automatic notification when temperature exceeds a threshold).  

  

# 🛏️ CribSense: Real-Time Infant Sleep Monitoring System

**CribSense** is a real-time embedded system based on Raspberry Pi, designed to monitor the sleep status of infants to ensure their safety. The system uses sensors to detect the infant’s movement, displays real-time data through a graphical interface, and provides alerts to notify caregivers of abnormal conditions.

## 📦 Project Structure
UofG-RTEP/ 

           ├── src/ # Core source code (C++)

           ├── chassis/ # Hardware interface and drivers 
           
           ├── docs/ # Documentation and design files 
           
           ├── cribsense/ # System services and configurations 
           
           ├── build-aux/ # Build automation tools 
           
           ├── Makefile # Build configuration 
           
           ├── LICENSE # Project license (Apache-2.0) 
           
           └── README.md # This documentation file

## 🚀 Getting Started

### 🖥️ System Requirements

- Raspberry Pi 3 (or newer)
- Raspberry Pi OS (Debian-based Linux)
- C++17-compatible compiler (e.g. `g++`)
- Development tools and libraries (e.g. `make`, `autoconf`, `libpthread`)

### 🔧 Installation Steps

1. Clone this repository:
   ```bash
   git clone https://github.com/Qicoco97/UofG-RTEP.git
   cd UofG-RTEP
2. Build the project:
   ```bash
   ./autogen.sh
   ./configure
   make
3. Install and start the service:
   ```bash
   sudo make install
   sudo systemctl enable cribsense
   sudo systemctl start cribsense

## 📈 Usage
After booting, the cribsense service starts automatically and begins monitoring.

Use the GUI to view real-time data such as infant motion and environmental temperature.

An alert is triggered when abnormal conditions (e.g., no motion detected for a long time) occur.

## 📸 Demo & Promotion
Figure:[figure] CribSense real-time monitoring interface

Project is published on GitHub and received positive feedback.

Shared progress and results on social media platforms (Twitter, LinkedIn, etc.)[link].

## 📄 License & Acknowledgements
This project is licensed under the Apache-2.0 license. You are welcome to use and modify it in compliance with the terms. Thanks to the University of Glasgow for course support and to all supervisors and peers who contributed ideas and feedback during the development of this project.


  



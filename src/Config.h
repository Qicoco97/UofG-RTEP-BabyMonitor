// Config.h - Centralized configuration constants
#pragma once

namespace BabyMonitorConfig {
    // GPIO Configuration
    constexpr int LED_CHIP_NUMBER = 0;
    constexpr int LED_PIN_NUMBER = 27;
    constexpr int DHT11_PIN_NUMBER = 17;
    
    // Motion Detection Configuration  
    constexpr double MOTION_MIN_AREA = 500.0;
    constexpr int MOTION_THRESHOLD = 25;
    
    // Timer Configuration
    constexpr int ALARM_TIMER_INTERVAL_MS = 1000;
    constexpr int DHT11_READ_INTERVAL_S = 3;  // Increased from 2s to 3s for better reliability
    
    // Chart Configuration
    constexpr int CHART_MAX_POINTS = 100;
    
    // LED Configuration
    constexpr int LED_BLINK_COUNT = 5;
    constexpr int LED_ON_DURATION_MS = 200;
    constexpr int LED_OFF_DURATION_MS = 100;
    
    // Camera Configuration
    constexpr int CAMERA_WIDTH = 640;
    constexpr int CAMERA_HEIGHT = 480;
    constexpr int CAMERA_FRAMERATE = 30;
    
    // Audio Configuration
    constexpr int NO_MOTION_ALARM_THRESHOLD = 5;  // Play alarm after 5 consecutive "No Motion" detections
    constexpr const char* ALARM_SOUND_FILE = "../img&demo/alarm.wav";

    // Device Paths
    constexpr const char* GPIO_CHIP_DEVICE = "/dev/gpiochip0";
}

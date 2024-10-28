#pragma once

namespace Config {
    // Display settings
    constexpr int SCREEN_WIDTH = 480;
    constexpr int SCREEN_HEIGHT = 320;
    constexpr int HEADER_HEIGHT = 30;
    
    // Sensor settings
    constexpr int NUM_POSITIONS = 10;
    constexpr int ADC_PIN = 29;
    constexpr int IR_FORK_PIN = 14;
    
    // I2C settings
    constexpr int I2C_DEFAULT_SPEED = 400000;
    constexpr int I2C_THERMAL_SPEED = 1000000;
    constexpr uint8_t I2C_SDA = 2;  // Added: I2C pins
    constexpr uint8_t I2C_SCL = 3;
    
    // Update intervals
    constexpr unsigned long DISPLAY_UPDATE_INTERVAL = 500;
    constexpr unsigned long GESTURE_CLEAR_INTERVAL = 2000;
    constexpr unsigned long SENSOR_POLL_INTERVAL = 10;
    
    // ADC calibration values
    constexpr std::array<int, NUM_POSITIONS> ADC_VALUES = {
        913, 800, 686, 572, 457, 344, 230, 117, 3, 1023
    };
}
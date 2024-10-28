#pragma once

#include <Arduino.h>

namespace Debug {
    // Print memory usage information
    inline void printMemoryInfo() {
        Serial.printf("Free Heap: %d bytes\n", rp2040.getFreeHeap());
    }

    // Print I2C status
    inline void printI2CInfo() {
        Serial.printf("I2C Clock: %d Hz\n", Wire1.getClock());
    }

    // Print CPU temperature
    inline void printCPUTemp() {
        float temp = analogReadTemp();
        Serial.printf("CPU Temperature: %.2f°C\n", temp);
    }

    // Print sensor switch position info
    inline void printSwitchInfo(int adcReading, int position) {
        Serial.printf("ADC Reading: %d, Position: %d\n", adcReading, position);
    }

    // Print full system status
    inline void printSystemStatus() {
        Serial.println("\n=== System Status ===");
        printMemoryInfo();
        printI2CInfo();
        printCPUTemp();
        Serial.println("==================\n");
    }
}
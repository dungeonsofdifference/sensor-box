#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "display/DisplayManager.h"
#include "sensors/SensorManager.h"

class SensorDemo {
private:
    // Order matters! Members are initialized in the order they are declared
    TFT_eSPI tft;             // Must be first - base display
    DisplayManager display;    // Depends on tft
    SensorManager sensorManager; // Depends on display

public:
    // Constructor explicitly shows dependencies
    SensorDemo();
    
    // Main program functions
    void setup();
    void loop();
    
    // Core 1 functions
    void setup1();
    void loop1();
    
    // Utility functions
    void reset();
    void printDebugInfo();
};
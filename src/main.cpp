#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "SensorDemo.h"

SensorDemo demo;

void setup() {
    // Initialize I2C for sensors
    Wire1.setSDA(Config::I2C_SDA);
    Wire1.setSCL(Config::I2C_SCL);
    Wire1.begin();
    Wire1.setClock(Config::I2C_DEFAULT_SPEED);
    
    // Initialize SPI for display
    SPI.begin();
    
    // Initialize the demo
    demo.setup();
}

void loop() {
    demo.loop();
}

void setup1() {
    demo.setup1();
}

void loop1() {
    demo.loop1();
}

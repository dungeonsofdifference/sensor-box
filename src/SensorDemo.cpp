#include "SensorDemo.h"
#include "config/config.h"

// Initialize in the same order as declared in the header
// Each member is initialized with its dependencies
SensorDemo::SensorDemo()
    : tft()                       // Initialize TFT first
    , display(tft)               // DisplayManager needs initialized TFT
    , sensorManager(display)     // SensorManager needs initialized DisplayManager
{
    // Constructor body empty - all initialization done in member initializer list
}

void SensorDemo::setup() {
    // Initialize Serial for debugging
    Serial.begin(115200);
    Serial.println("Starting Sensor Demo...");

    // Initialize I2C for sensors
    Wire1.setSDA(Config::I2C_SDA);
    Wire1.setSCL(Config::I2C_SCL);
    Wire1.begin();
    Wire1.setClock(Config::I2C_DEFAULT_SPEED);
    
    // Initialize SPI for display
    SPI.begin();
    
    // Initialize ADC pin for rotary switch
    pinMode(Config::ADC_PIN, INPUT);
    
    // Initialize display
    display.initialize();
    Serial.println("Display initialized");

    // Initialize sensors
    if (!sensorManager.setup()) {
        Serial.println("Warning: Some sensors failed to initialize");
    } else {
        Serial.println("All sensors initialized successfully");
    }

    Serial.println("Setup complete!");
}

void SensorDemo::loop() {
    sensorManager.loop();
}

void SensorDemo::setup1() {
    Serial.println("Core 1 starting...");
    sensorManager.setup1();
}

void SensorDemo::loop1() {
    sensorManager.loop1();
}

// Helper method to reset everything if needed
void SensorDemo::reset() {
    Wire1.setClock(Config::I2C_DEFAULT_SPEED);
    display.initialize();
    sensorManager.setup();
}

// Helper method for debug info
void SensorDemo::printDebugInfo() {
    Serial.println("\nDebug Information:");
    Serial.println("-----------------");
    Serial.print("I2C Clock: "); 
    Serial.println(Wire1.getClock());
    Serial.print("Free Memory: "); 
    Serial.println(rp2040.getFreeHeap());
    Serial.print("CPU Temperature: "); 
    Serial.println(analogReadTemp());
}
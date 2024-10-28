#include "Sensor.h"

Sensor::Sensor(DisplayManager& displayManager) : display(displayManager) {}

bool Sensor::isInitialized() const {
    return initialized;
}

bool Sensor::shouldUpdate(unsigned long interval) {
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}
#pragma once

#include <Arduino.h>
#include "DisplayManager.h"

class Sensor {
protected:
    bool initialized = false;
    unsigned long lastUpdateTime = 0;
    DisplayManager& display;

public:
    explicit Sensor(DisplayManager& displayManager);
    virtual ~Sensor() = default;

    virtual bool setup() = 0;
    virtual void prepare() = 0;
    virtual void updateDisplay() = 0;
    
    bool isInitialized() const;
    virtual const char* getName() const = 0;
    
protected:
    bool shouldUpdate(unsigned long interval);
};
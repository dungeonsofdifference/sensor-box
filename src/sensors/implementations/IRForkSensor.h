#pragma once

#include "Sensor.h"
#include "config.h"

class IRFork1 : public Sensor {
private:
    bool priorVal = HIGH;

public:
    explicit IRFork1(DisplayManager& display);
    bool setup() override;
    void prepare() override;
    void updateDisplay() override;
    const char* getName() const override;
};

class IRFork2 : public Sensor {
private:
    bool priorVal = HIGH;
    int count = 0;

public:
    explicit IRFork2(DisplayManager& display);
    bool setup() override;
    void prepare() override;
    void updateDisplay() override;
    const char* getName() const override;
};

class IRFork3 : public Sensor {
private:
    bool priorVal = HIGH;
    unsigned long lastDetectionTime = 0;
    unsigned long currentDetectionTime = 0;

public:
    explicit IRFork3(DisplayManager& display);
    bool setup() override;
    void prepare() override;
    void updateDisplay() override;
    const char* getName() const override;
};
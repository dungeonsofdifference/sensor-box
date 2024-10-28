#pragma once

#include "Sensor.h"
#include <DFRobot_PAJ7620U2.h>

class GestureSensor : public Sensor {
private:
    DFRobot_PAJ7620U2 paj;
    unsigned long lastGestureTime = 0;
    static constexpr unsigned long GESTURE_TIMEOUT = 2000; // 2 seconds

public:
    explicit GestureSensor(DisplayManager& display);
    bool setup() override;
    void prepare() override;
    void updateDisplay() override;
    const char* getName() const override;

private:
    void handleGesture(DFRobot_PAJ7620U2::eGesture_t gesture);
    void clearDisplayIfTimeout();
};
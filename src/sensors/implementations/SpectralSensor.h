#pragma once

#include "Sensor.h"
#include <Adafruit_AS7341.h>

class SpectralSensor : public Sensor {
private:
    Adafruit_AS7341 as7341;
    bool stateLED = false;
    bool priorVal = HIGH;
    unsigned long previousMillis = 0;
    static constexpr unsigned long UPDATE_INTERVAL = 500;

public:
    explicit SpectralSensor(DisplayManager& display);
    bool setup() override;
    void prepare() override;
    void updateDisplay() override;
    const char* getName() const override;

private:
    void toggleLED();
    void displayReadings(uint16_t* readings);
    void setupSensorConfig();
    void drawLabels();
};
#pragma once

#include "Sensor.h"
#include <Adafruit_MLX90640.h>

class ThermalSensor : public Sensor {
private:
    Adafruit_MLX90640 mlx;
    float frame[32 * 24];
    static const uint16_t camColors[256];
    static constexpr int MINTEMP = 20;
    static constexpr int MAXTEMP = 35;
    static constexpr int DISPLAY_START_X = 80;
    static constexpr int DISPLAY_START_Y = 50;
    
public:
    explicit ThermalSensor(DisplayManager& display);
    bool setup() override;
    void prepare() override;
    void updateDisplay() override;
    const char* getName() const override;

private:
    void displayThermalFrame();
    uint16_t getColorForTemp(float temp);
};
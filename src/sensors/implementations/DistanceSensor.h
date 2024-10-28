#pragma once

#include "Sensor.h"
#include <vl53l4cx_class.h>

class DistanceSensor : public Sensor {
private:
    VL53L4CX sensor_vl53l4cx_sat;
    unsigned long previousMillis = 0;
    static constexpr unsigned long UPDATE_INTERVAL = 100;

public:
    explicit DistanceSensor(DisplayManager& display);
    bool setup() override;
    void prepare() override;
    void updateDisplay() override;
    const char* getName() const override;

private:
    void processRangeData(VL53L4CX_MultiRangingData_t& data);
};
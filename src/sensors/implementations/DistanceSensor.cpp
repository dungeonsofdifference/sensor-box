#include "DistanceSensor.h"
#include "config.h"

DistanceSensor::DistanceSensor(DisplayManager& display) : Sensor(display) {}

bool DistanceSensor::setup() {
    sensor_vl53l4cx_sat.setI2cDevice(&Wire1);
    if (!sensor_vl53l4cx_sat.begin()) {
        Serial.println("Failed to initialize VL53L4CX!");
        return false;
    }

    VL53L4CX_Error error = sensor_vl53l4cx_sat.InitSensor(VL53L4CX_DEFAULT_DEVICE_ADDRESS);
    if (error != VL53L4CX_ERROR_NONE) {
        Serial.printf("Error Initializing Sensor: %d\n", error);
        return false;
    }

    sensor_vl53l4cx_sat.VL53L4CX_StartMeasurement();
    initialized = true;
    return true;
}

void DistanceSensor::prepare() {
    display.drawHeader("Time of Flight Distance");
    display.clearDisplayArea();
    Wire1.setClock(Config::I2C_DEFAULT_SPEED);
}

void DistanceSensor::updateDisplay() {
    if (!shouldUpdate(UPDATE_INTERVAL)) return;

    VL53L4CX_MultiRangingData_t MultiRangingData;
    uint8_t NewDataReady = 0;
    int status;

    do {
        status = sensor_vl53l4cx_sat.VL53L4CX_GetMeasurementDataReady(&NewDataReady);
    } while (!NewDataReady);

    if ((!status) && (NewDataReady != 0)) {
        status = sensor_vl53l4cx_sat.VL53L4CX_GetMultiRangingData(&MultiRangingData);
        processRangeData(MultiRangingData);
        
        if (status == 0) {
            sensor_vl53l4cx_sat.VL53L4CX_ClearInterruptAndStartMeasurement();
        }
    }
}

void DistanceSensor::processRangeData(VL53L4CX_MultiRangingData_t& data) {
    for (int j = 0; j < data.NumberOfObjectsFound; j++) {
        if (data.RangeData[j].RangeStatus == VL53L4CX_RANGESTATUS_RANGE_VALID ||
            data.RangeData[j].RangeStatus == VL53L4CX_RANGESTATUS_RANGE_VALID_MERGED_PULSE) {
            
            char distanceStr[16];
            snprintf(distanceStr, sizeof(distanceStr), "%d mm", 
                    data.RangeData[j].RangeMilliMeter);
            display.drawCenterText(distanceStr);
            Serial.println(distanceStr);
        }
    }
}

const char* DistanceSensor::getName() const {
    return "Distance Sensor";
}
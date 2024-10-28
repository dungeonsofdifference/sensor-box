#include "SensorManager.h"
#include "implementations/IRForkSensor.h"
#include "implementations/GestureSensor.h"
#include "implementations/DistanceSensor.h"
#include "implementations/SpectralSensor.h"
#include "implementations/ThermalSensor.h"

SensorManager::SensorManager(DisplayManager& displayManager)
    : display(displayManager)
    , currentPosition(0)
    , lastAdcPosition(0)
{
    initializeSensorArray();
}

void SensorManager::initializeSensorArray() {
    sensors[0] = std::make_unique<IRFork1>(display);
    sensors[1] = std::make_unique<IRFork2>(display);
    sensors[2] = std::make_unique<IRFork3>(display);
    sensors[3] = std::make_unique<GestureSensor>(display);
    sensors[4] = std::make_unique<DistanceSensor>(display);
    sensors[5] = std::make_unique<SpectralSensor>(display);
    sensors[6] = std::make_unique<ThermalSensor>(display);
}

bool SensorManager::initializeSensors() {
    bool allInitialized = true;
    for (auto& sensor : sensors) {
        if (sensor && !sensor->setup()) {
            Serial.printf("Failed to initialize %s\n", sensor->getName());
            allInitialized = false;
        }
    }
    return allInitialized;
}

void SensorManager::pollSwitchPosition() {
    int position = getSwitchPosition();
    if (position != lastAdcPosition) {
        while (!rp2040.fifo.push_nb(static_cast<uint32_t>(position))) {
            delay(1);
        }
        lastAdcPosition = position;
    }
}

void SensorManager::processFifoMessages() {
    uint32_t newPosition;
    if (rp2040.fifo.pop_nb(&newPosition)) {
        handlePositionChange(newPosition);
    }
}

void SensorManager::handlePositionChange(int newPosition) {
    if (newPosition == currentPosition) return;
    
    if (newPosition >= 1 && newPosition <= Config::NUM_POSITIONS) {
        auto& sensor = sensors[newPosition - 1];
        if (sensor) {
            // Set appropriate I2C speed
            Wire1.setClock(
                (dynamic_cast<ThermalSensor*>(sensor.get()) != nullptr) 
                ? Config::I2C_THERMAL_SPEED 
                : Config::I2C_DEFAULT_SPEED
            );
            
            sensor->prepare();
            currentPosition = newPosition;
            Serial.printf("Switched to sensor %d: %s\n", 
                        currentPosition, sensor->getName());
        }
    }
}

void SensorManager::updateCurrentSensor() {
    if (currentPosition >= 1 && currentPosition <= Config::NUM_POSITIONS) {
        auto& sensor = sensors[currentPosition - 1];
        if (sensor && sensor->isInitialized()) {
            sensor->updateDisplay();
        }
    }
}

int SensorManager::getSwitchPosition() {
    int adcReading = analogRead(Config::ADC_PIN);
    int closestPosition = 1;
    int minDifference = abs(adcReading - Config::ADC_VALUES[0]);

    for (int i = 1; i < Config::NUM_POSITIONS; i++) {
        int difference = abs(adcReading - Config::ADC_VALUES[i]);
        if (difference < minDifference) {
            minDifference = difference;
            closestPosition = i + 1;
        }
    }
    return closestPosition;
}
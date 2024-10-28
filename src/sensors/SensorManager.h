#pragma once

#include <array>
#include <memory>
#include "Sensor.h"
#include "../config/config.h"
#include "../display/DisplayManager.h"

class SensorManager {
private:
    std::array<std::unique_ptr<Sensor>, Config::NUM_POSITIONS> sensors;
    DisplayManager& display;
    int currentPosition = 0;
    int lastAdcPosition = 0;
    static constexpr unsigned long POLL_INTERVAL = 10; // ms
    static constexpr std::array<int, Config::NUM_POSITIONS> ADC_VALUES = {
        913, 800, 686, 572, 457, 344, 230, 117, 3, 1023
    };

public:
    explicit SensorManager(DisplayManager& displayManager)
        : display(displayManager) {
        initializeSensorArray();
    }

    // Core0 (main core) methods
    bool setup() {
        return initializeSensors();
    }

    void loop() {
        processFifoMessages();
        updateCurrentSensor();
    }

    // Core1 methods
    void setup1() {
        delay(100); // Initial delay for core stability
    }

    void loop1() {
        pollSwitchPosition();
        delay(POLL_INTERVAL);
    }

private:
    void initializeSensorArray() {
        sensors[0] = std::make_unique<IRFork1>(display);
        sensors[1] = std::make_unique<IRFork2>(display);
        sensors[2] = std::make_unique<IRFork3>(display);
        sensors[3] = std::make_unique<GestureSensor>(display);
        sensors[4] = std::make_unique<DistanceSensor>(display);
        sensors[5] = std::make_unique<SpectralSensor>(display);
        sensors[6] = std::make_unique<ThermalSensor>(display);
    }

    bool initializeSensors() {
        bool allInitialized = true;
        for (auto& sensor : sensors) {
            if (sensor && !sensor->setup()) {
                Serial.printf("Failed to initialize %s\n", sensor->getName());
                allInitialized = false;
            }
        }
        return allInitialized;
    }

    void pollSwitchPosition() {
        int position = getSwitchPosition();
        if (position != lastAdcPosition) {
            while (!rp2040.fifo.push_nb(static_cast<uint32_t>(position))) {
                // Wait until FIFO has space
                delay(1);
            }
            lastAdcPosition = position;
        }
    }

    void processFifoMessages() {
        uint32_t newPosition;
        if (rp2040.fifo.pop_nb(&newPosition)) {
            handlePositionChange(newPosition);
        }
    }

    void handlePositionChange(int newPosition) {
        if (newPosition == currentPosition) return;
        
        if (newPosition >= 1 && newPosition <= Config::NUM_POSITIONS) {
            auto& sensor = sensors[newPosition - 1];
            if (sensor) {
                sensor->prepare();
                currentPosition = newPosition;
                Serial.printf("Switched to sensor %d: %s\n", 
                            currentPosition, sensor->getName());
            }
        }
    }

    void updateCurrentSensor() {
        if (currentPosition >= 1 && currentPosition <= Config::NUM_POSITIONS) {
            auto& sensor = sensors[currentPosition - 1];
            if (sensor && sensor->isInitialized()) {
                sensor->updateDisplay();
            }
        }
    }

    int getSwitchPosition() {
        int adcReading = analogRead(Config::ADC_PIN);
        int closestPosition = 1;
        int minDifference = abs(adcReading - ADC_VALUES[0]);

        for (int i = 1; i < Config::NUM_POSITIONS; i++) {
            int difference = abs(adcReading - ADC_VALUES[i]);
            if (difference < minDifference) {
                minDifference = difference;
                closestPosition = i + 1;
            }
        }
        return closestPosition;
    }
};
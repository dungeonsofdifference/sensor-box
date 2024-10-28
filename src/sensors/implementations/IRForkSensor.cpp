#include "IRForkSensor.h"

// IRFork1 Implementation
IRFork1::IRFork1(DisplayManager& display) : Sensor(display) {}

bool IRFork1::setup() {
    pinMode(Config::IR_FORK_PIN, INPUT_PULLUP);
    initialized = true;
    return true;
}

void IRFork1::prepare() {
    display.drawHeader("IR Break Beam");
    display.clearDisplayArea();
}

void IRFork1::updateDisplay() {
    int sensorVal = digitalRead(Config::IR_FORK_PIN);
    if (sensorVal != priorVal) {
        display.drawCenterText(sensorVal == LOW ? "BLOCKED" : "CLEAR");
        Serial.println(sensorVal == LOW ? "BLOCKED" : "CLEAR");
        priorVal = sensorVal;
    }
}

const char* IRFork1::getName() const {
    return "IR Fork 1";
}

// IRFork2 Implementation
IRFork2::IRFork2(DisplayManager& display) : Sensor(display) {}

bool IRFork2::setup() {
    pinMode(Config::IR_FORK_PIN, INPUT_PULLUP);
    initialized = true;
    return true;
}

void IRFork2::prepare() {
    display.drawHeader("IR Fork - Counts");
    display.clearDisplayArea();
}

void IRFork2::updateDisplay() {
    int sensorVal = digitalRead(Config::IR_FORK_PIN);
    if (sensorVal != priorVal) {
        if (sensorVal == LOW) {
            count++;
            char countStr[16];
            snprintf(countStr, sizeof(countStr), "%d", count);
            display.drawCenterText(countStr);
            Serial.println(countStr);
        }
        priorVal = sensorVal;
    }
}

const char* IRFork2::getName() const {
    return "IR Fork 2";
}

// IRFork3 Implementation
IRFork3::IRFork3(DisplayManager& display) : Sensor(display) {}

bool IRFork3::setup() {
    pinMode(Config::IR_FORK_PIN, INPUT_PULLUP);
    initialized = true;
    return true;
}

void IRFork3::prepare() {
    display.drawHeader("IR Fork - Timer");
    display.clearDisplayArea();
}

void IRFork3::updateDisplay() {
    int sensorVal = digitalRead(Config::IR_FORK_PIN);
    if (sensorVal != priorVal) {
        if (sensorVal == LOW) {
            currentDetectionTime = millis();
            if (lastDetectionTime != 0) {
                unsigned long timeElapsed = currentDetectionTime - lastDetectionTime;
                char timeElapsedStr[32];
                snprintf(timeElapsedStr, sizeof(timeElapsedStr), "%lu ms", timeElapsed);
                display.drawCenterText(timeElapsedStr);
                Serial.println(timeElapsedStr);
            }
            lastDetectionTime = currentDetectionTime;
        }
        priorVal = sensorVal;
    }
}

const char* IRFork3::getName() const {
    return "IR Fork 3";
}
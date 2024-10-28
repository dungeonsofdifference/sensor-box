#include "GestureSensor.h"
#include "config.h"

GestureSensor::GestureSensor(DisplayManager& display) : Sensor(display) {}

bool GestureSensor::setup() {
    delay(300);
    if (paj.begin() != 0) {
        Serial.println("PAJ7620U2 failed to initialize!");
        return false;
    }
    Serial.println("PAJ7620U2 initialized!");
    paj.setGestureHighRate(true);
    initialized = true;
    return true;
}

void GestureSensor::prepare() {
    display.drawHeader("Gesture Sensor");
    display.clearDisplayArea();
    Wire1.setClock(Config::I2C_DEFAULT_SPEED);
    paj.begin();
}

void GestureSensor::updateDisplay() {
    DFRobot_PAJ7620U2::eGesture_t gesture = paj.getGesture();
    if (gesture != paj.eGestureNone) {
        handleGesture(gesture);
    }
    clearDisplayIfTimeout();
}

void GestureSensor::handleGesture(DFRobot_PAJ7620U2::eGesture_t gesture) {
    lastGestureTime = millis();
    const char* gestureText = nullptr;
    
    switch (gesture) {
        case DFRobot_PAJ7620U2::eGestureRight:  gestureText = "Right"; break;
        case DFRobot_PAJ7620U2::eGestureLeft:   gestureText = "Left"; break;
        case DFRobot_PAJ7620U2::eGestureUp:     gestureText = "Up"; break;
        case DFRobot_PAJ7620U2::eGestureDown:   gestureText = "Down"; break;
        case DFRobot_PAJ7620U2::eGestureForward: gestureText = "Forward"; break;
        case DFRobot_PAJ7620U2::eGestureBackward: gestureText = "Backward"; break;
        case DFRobot_PAJ7620U2::eGestureClockwise: gestureText = "CW"; break;
        case DFRobot_PAJ7620U2::eGestureAntiClockwise: gestureText = "CCW"; break;
        default: return;
    }
    
    display.drawCenterText(gestureText);
    Serial.println(gestureText);
}

void GestureSensor::clearDisplayIfTimeout() {
    if (millis() - lastGestureTime > GESTURE_TIMEOUT) {
        display.clearDisplayArea();
        lastGestureTime = millis();
    }
}

const char* GestureSensor::getName() const {
    return "Gesture Sensor";
}
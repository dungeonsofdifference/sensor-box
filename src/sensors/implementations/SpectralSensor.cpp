#include "SpectralSensor.h"
#include "config.h"

SpectralSensor::SpectralSensor(DisplayManager& display) : Sensor(display) {}

bool SpectralSensor::setup() {
    if (!as7341.begin(AS7341_I2CADDR_DEFAULT, &Wire1)) {
        Serial.println("Could not find AS7341");
        return false;
    }
    Serial.println("AS7341 Connected!");
    setupSensorConfig();
    initialized = true;
    return true;
}

void SpectralSensor::setupSensorConfig() {
    as7341.setATIME(100);
    as7341.setASTEP(100);
    as7341.setGain(AS7341_GAIN_256X);
    as7341.setLEDCurrent(4);
}

void SpectralSensor::prepare() {
    display.drawHeader("Spectrometer");
    display.clearDisplayArea();
    Wire1.setClock(Config::I2C_DEFAULT_SPEED);
    as7341.enableLED(false);
    drawLabels();
}

void SpectralSensor::drawLabels() {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(4);
    tft.setCursor(120, 90);
    tft.print("445nm: ");
    tft.setCursor(120, 130);
    tft.print("555nm: ");
    tft.setCursor(120, 170);
    tft.print("680nm: ");
}

void SpectralSensor::updateDisplay() {
    int sensorVal = digitalRead(Config::IR_FORK_PIN);
    toggleLED();

    uint16_t readings[12];
    if (!as7341.readAllChannels(readings)) {
        Serial.println("Error reading all channels!");
        return;
    }

    if (shouldUpdate(UPDATE_INTERVAL)) {
        displayReadings(readings);
    }
}

void SpectralSensor::toggleLED() {
    int sensorVal = digitalRead(Config::IR_FORK_PIN);
    if (sensorVal != priorVal) {
        if (sensorVal == LOW) {
            stateLED = !stateLED;
            as7341.enableLED(stateLED);
        }
        priorVal = sensorVal;
    }
}

void SpectralSensor::displayReadings(uint16_t* readings) {
    int padding = tft.textWidth("123nm: 12345");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextPadding(padding);
    tft.setTextSize(4);

    // Update display for each wavelength
    const int xOffset = 120 + tft.textWidth("123 nm:");
    const int yPositions[] = {90, 130, 170};
    const int readingIndices[] = {1, 4, 7}; // Blue, Green, Red

    for (int i = 0; i < 3; i++) {
        tft.setCursor(xOffset, yPositions[i]);
        tft.print("     ");
        tft.setCursor(xOffset, yPositions[i]);
        tft.print(readings[readingIndices[i]]);
    }

    // Log to serial
    Serial.printf("445nm: %d, 555nm: %d, 680nm: %d\n",
                 readings[1], readings[4], readings[7]);
}

const char* SpectralSensor::getName() const {
    return "Spectral Sensor";
}
#include "DisplayManager.h"

DisplayManager::DisplayManager(TFT_eSPI& tftDisplay) 
    : tft(tftDisplay)
    , displayWidth(Config::SCREEN_WIDTH)
    , displayHeight(Config::SCREEN_HEIGHT) 
{}

void DisplayManager::initialize() {
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(Config::BACKGROUND_COLOR);
    drawVerticalNumbers();
}

void DisplayManager::drawHeader(const char* text) {
    tft.fillRect(0, 0, displayWidth, Config::HEADER_HEIGHT, Config::BACKGROUND_COLOR);
    setupTextForHeader();
    tft.drawString(text, displayWidth / 2, 5);
}

void DisplayManager::drawCenterText(const char* text, uint16_t textColor) {
    setupTextForCenter();
    int padding = tft.textWidth("Forwards");  // Maximum expected width
    tft.setTextPadding(padding);
    tft.setTextColor(textColor, Config::BACKGROUND_COLOR);
    tft.drawString(text, displayWidth / 2, displayHeight / 2);
}

void DisplayManager::clearDisplayArea() {
    tft.fillRect(80, 50, displayWidth - 80, displayHeight - 50, Config::BACKGROUND_COLOR);
}

void DisplayManager::drawVerticalNumbers() {
    tft.setTextSize(2);
    tft.setTextColor(Config::TEXT_COLOR);
    for (int i = 1; i <= Config::NUM_POSITIONS; i++) {
        tft.setCursor(10, i * 30);
        tft.print(i);
    }
}

void DisplayManager::drawArrow(int position) {
    int ypos = (position * 30) + 5;
    tft.fillTriangle(40, ypos, 50, ypos - 5, 50, ypos + 5, Config::TEXT_COLOR);
}

void DisplayManager::eraseArrow(int position) {
    int ypos = (position * 30) + 5;
    tft.fillRect(34, ypos - 6, 22, 12, Config::BACKGROUND_COLOR);
}

void DisplayManager::setupTextForHeader() {
    tft.setTextSize(3);
    tft.setTextColor(Config::HEADER_COLOR);
    tft.setTextDatum(TC_DATUM);
}

void DisplayManager::setupTextForCenter() {
    tft.setTextDatum(TC_DATUM);
    tft.setTextSize(8);
}
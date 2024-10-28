#pragma once

#include <TFT_eSPI.h>
#include "config.h"

class DisplayManager {
private:
    TFT_eSPI& tft;
    int displayWidth;
    int displayHeight;

public:
    explicit DisplayManager(TFT_eSPI& tftDisplay);
    void initialize();
    void drawHeader(const char* text);
    void drawCenterText(const char* text, uint16_t textColor = Config::TEXT_COLOR);
    void clearDisplayArea();
    void drawVerticalNumbers();
    void drawArrow(int position);
    void eraseArrow(int position);

private:
    void setupTextForHeader();
    void setupTextForCenter();
};
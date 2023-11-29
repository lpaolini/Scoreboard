#include "GenericDisplay.h"

GenericDisplay::GenericDisplay(
    LedControl *display, 
    uint8_t displayIndex,
    uint8_t brightness
) {
    this->display = display;
    this->displayIndex = displayIndex;
    this->brightness = brightness;
    display->shutdown(displayIndex, false);
    display->clearDisplay(displayIndex);
    display->setIntensity(displayIndex, brightness);
}

void GenericDisplay::set(uint8_t v1, uint8_t v2) {
    this->v1 = v1;
    this->v2 = v2;
    update();
}

void GenericDisplay::update() {
    display->setDigit(displayIndex, 7, decimalDigit(v1, 2), false);
    display->setDigit(displayIndex, 6, decimalDigit(v1, 1), false);
    display->setDigit(displayIndex, 5, decimalDigit(v1, 0), false);
    display->setDigit(displayIndex, 2, decimalDigit(v2, 2), false);
    display->setDigit(displayIndex, 1, decimalDigit(v2, 1), false);
    display->setDigit(displayIndex, 0, decimalDigit(v2, 0), false);
}

int GenericDisplay::decimalDigit(int value, int digit) {
    switch (digit) {
        case 0: return abs(value) % 10;
        case 1: return abs(value) / 10 % 10;
        case 2: return abs(value) / 100 % 10;
        default: return 0;
    }
}

void GenericDisplay::loop() {
}

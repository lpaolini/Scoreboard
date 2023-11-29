#include "ParamDisplay.h"

ParamDisplay::ParamDisplay(
    LedControl *display, 
    uint8_t displayIndex,
    uint8_t brightness,
    void (*onParamsUpdate)(uint8_t (&value)[PARAM_COUNT])
) {
    this->display = display;
    this->displayIndex = displayIndex;
    this->brightness = brightness;
    this->onParamsUpdate = onParamsUpdate;
    display->shutdown(displayIndex, false);
    display->clearDisplay(displayIndex);
    display->setIntensity(displayIndex, brightness);
}

void ParamDisplay::setup() {
    update();
}

int ParamDisplay::decimalDigit(int value, int digit) {
    switch (digit) {
        case 0: return abs(value) % 10;
        case 1: return abs(value) / 10 % 10;
        case 2: return abs(value) / 100 % 10;
        default: return 0;
    }
}

void ParamDisplay::update() {
    display->setDigit(displayIndex, 7, decimalDigit(field, 2), false);
    display->setDigit(displayIndex, 6, decimalDigit(field, 1), false);
    display->setDigit(displayIndex, 5, decimalDigit(field, 0), false);
    display->setDigit(displayIndex, 2, decimalDigit(value[field], 2), false);
    display->setDigit(displayIndex, 1, decimalDigit(value[field], 1), false);
    display->setDigit(displayIndex, 0, decimalDigit(value[field], 0), false);
    onParamsUpdate(value);
}

void ParamDisplay::increaseField() {
    field = (field + 1) % (maxField + 1);
    update();
}

void ParamDisplay::decreaseField() {
    field = (field + maxField) % (maxField + 1);
    update();
}

void ParamDisplay::resetField() {
    field = 0;
    update();
}

void ParamDisplay::increaseValue() {
    value[field] = (value[field] + 1) % (maxValue[field] + 1);
    update();
}

void ParamDisplay::decreaseValue() {
    value[field] = (value[field] + maxValue[field]) % (maxValue[field] + 1);
    update();
}

void ParamDisplay::resetValue() {
    value[field] = 0;
    update();
}

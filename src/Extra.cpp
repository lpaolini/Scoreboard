#include "Extra.h"

Extra::Extra(
    LedControl *display, 
    uint8_t displayIndex,
    uint8_t brightness,
    bool invert,
    void (*onUpdateFouls)(uint8_t fouls),
    void (*onUpdateTimeouts)(uint8_t timeouts)
) {
    this->display = display;
    this->displayIndex = displayIndex;
    this->brightness = brightness;
    this->invert = invert;
    this->onUpdateFouls = onUpdateFouls;
    this->onUpdateTimeouts = onUpdateTimeouts;
}

void Extra::setup() {
    display->shutdown(displayIndex, false);
    display->clearDisplay(displayIndex);
    display->setIntensity(displayIndex, brightness);
    reset();
}

void Extra::reset() {
    updating = false;
    inputTimer.stop();
    resetFouls();
    resetTimeouts();
}

void Extra::resetPeriod() {
    updating = false;
    inputTimer.stop();
    resetFouls();
}

void Extra::resetFouls() {
    fouls = 0;
    prevFouls = 0;
    foulsFlashTimer.stop();
    updateFouls();
}

void Extra::resetTimeouts() {
    timeouts = 0;
    prevTimeouts = 0;
    timeoutsFlashTimer.stop();
    updateTimeouts();
}

void Extra::printTimeoutChar(uint8_t pos, bool show) {
    if (show) {
        display->setLed(displayIndex, pos, 3, true);
        display->setLed(displayIndex, pos, 4, true);
        display->setLed(displayIndex, pos, 5, true);
        display->setLed(displayIndex, pos, 7, true);
    } else {
        display->setChar(displayIndex, pos, ' ', false);
    }
}

void Extra::updateFouls(bool show) {
    if (show) {
        display->setChar(displayIndex, FOULS_POS[invert][2], 'F', false);
        uint8_t tenths = decimalDigit(fouls, 1);
        if (tenths) {
            display->setDigit(displayIndex, FOULS_POS[invert][1], tenths, false);
        } else {
            display->setChar(displayIndex, FOULS_POS[invert][1], ' ', false);
        }
        display->setDigit(displayIndex, FOULS_POS[invert][0], decimalDigit(fouls, 0), false);
    } else {
        display->setChar(displayIndex, FOULS_POS[invert][2], ' ', false);
        display->setChar(displayIndex, FOULS_POS[invert][1], ' ', false);
        display->setChar(displayIndex, FOULS_POS[invert][0], ' ', false);
    }
}

void Extra::updateTimeouts(bool show) {
    if (show) {
        printTimeoutChar(TIMEOUTS_POS[invert][2], timeouts > 2);
        printTimeoutChar(TIMEOUTS_POS[invert][1], timeouts > 1);
        printTimeoutChar(TIMEOUTS_POS[invert][0], timeouts > 0);
    } else {
        display->setChar(displayIndex, TIMEOUTS_POS[invert][2], ' ', false);
        display->setChar(displayIndex, TIMEOUTS_POS[invert][1], ' ', false);
        display->setChar(displayIndex, TIMEOUTS_POS[invert][0], ' ', false);
    }
}

void Extra::update() {
    updateFouls();
    updateTimeouts();
}

void Extra::increaseFouls() {
    fouls = min(fouls + 1, MAX_FOULS);
    inputTimer.reset();
    updating = true;
    update();
}

void Extra::decreaseFouls() {
    fouls = max(fouls - 1, 0);
    inputTimer.reset();
    updating = true;
    update();
}

void Extra::increaseTimeouts() {
    timeouts = min(timeouts + 1, MAX_TIMEOUTS);
    inputTimer.reset();
    updating = true;
    update();
}

void Extra::decreaseTimeouts() {
    timeouts = max(timeouts - 1, 0);
    inputTimer.reset();
    updating = true;
    update();
}

int Extra::decimalDigit(int value, int digit) {
    switch (digit) {
        case 0: return abs(value) % 10;
        case 1: return abs(value) / 10 % 10;
        case 2: return abs(value) / 100 % 10;
        default: return 0;
    }
}

void Extra::loopInput() {
    inputTimer.loop();
    if (updating) {
        if (inputTimer.isTriggered()) {
            if (fouls != prevFouls) {
                updateFouls();
                onUpdateFouls(fouls);
                prevFouls = fouls;
                foulsFlashTimer.reset();
            }
            if (timeouts != prevTimeouts) {
                updateTimeouts();
                onUpdateTimeouts(timeouts);
                prevTimeouts = timeouts;
                timeoutsFlashTimer.reset();
            }
            updating = false;
        }
    }
}

void Extra::loopFoulsFlash() {
    foulsFlashTimer.loop();
    if (foulsFlashTimer.isRunning()) {
        updateFouls(foulsFlashTimer.elapsed() / CONFIRMATION_FLASH_DURATION_MS % 2 == 0);
    } else {
        updateFouls();
    }
}

void Extra::loopTimeoutsFlash() {
    timeoutsFlashTimer.loop();
    if (timeoutsFlashTimer.isRunning()) {
        updateTimeouts(timeoutsFlashTimer.elapsed() / CONFIRMATION_FLASH_DURATION_MS % 2 == 0);
    } else {
        updateTimeouts();
    }
}

void Extra::loop() {
    loopInput();
    loopFoulsFlash();
    loopTimeoutsFlash();
}

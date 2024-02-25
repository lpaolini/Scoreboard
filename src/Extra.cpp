#include "Extra.h"

Extra::Extra(
    LedControl *display, 
    uint8_t displayIndex,
    uint8_t brightness,
    bool invert,
    Beeper *beeper
) {
    this->display = display;
    this->displayIndex = displayIndex;
    this->brightness = brightness;
    this->invert = invert;
    this->beeper = beeper;
}

void Extra::setup(
    void (*onUpdateFouls)(uint8_t fouls), 
    void (*onUpdateTimeouts)(uint8_t timeouts)
) {
    this->onUpdateFouls = onUpdateFouls;
    this->onUpdateTimeouts = onUpdateTimeouts;
    display->shutdown(displayIndex, true);
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

void Extra::setPeriod(uint8_t period) {
    updating = false;
    inputTimer.stop();
    resetFouls();
    if (period == 1 || period == 3 || period == 5 || period == 6) {
        resetTimeouts();
    }
    maxTimeouts = getMaxTimeouts(period);
    updateFoulsDisplay();
    updateTimeoutsDisplay();
    enable();
}

uint8_t Extra::getMaxTimeouts(uint8_t period) {
    if (period == 1 || period == 2) return 2;
    if (period == 3 || period == 4) return 3;
    if (period == 5) return 1;
    return 0;
}

void Extra::enable() {
    display->shutdown(displayIndex, false);
    enabled = true;
}

void Extra::disable() {
    display->shutdown(displayIndex, true);
    enabled = false;
}

void Extra::printTimeoutChar(uint8_t pos, bool show) {
    if (show) {
        display->setLed(displayIndex, pos, 3, true);
        display->setLed(displayIndex, pos, 4, true);
        display->setLed(displayIndex, pos, 5, true);
        display->setLed(displayIndex, pos, 7, true);
    } else {
        display->setChar(displayIndex, pos, '_', false);
    }
}

void Extra::updateFoulsDisplay(bool show) {
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

void Extra::updateTimeoutsDisplay(bool show) {
    if (show && maxTimeouts > 2) {
        printTimeoutChar(TIMEOUTS_POS[invert][2], timeouts > 2);
    } else {
        display->setChar(displayIndex, TIMEOUTS_POS[invert][2], ' ', false);
    }
    if (show && maxTimeouts > 1) {
        printTimeoutChar(TIMEOUTS_POS[invert][1], timeouts > 1);
    } else {
        display->setChar(displayIndex, TIMEOUTS_POS[invert][1], ' ', false);
    }
    if (show && maxTimeouts > 0) {
        printTimeoutChar(TIMEOUTS_POS[invert][0], timeouts > 0);
    } else {
        display->setChar(displayIndex, TIMEOUTS_POS[invert][0], ' ', false);
    }
}

void Extra::resetFouls() {
    updateFouls(0, true);
}

void Extra::increaseFouls() {
    if (fouls < MAX_FOULS) {
        updateFouls(fouls + 1);
    } else {
        beeper->notAllowed();
    }
}

void Extra::decreaseFouls() {
    if (fouls > 0) {
        updateFouls(max(fouls - 1, 0));
    } else {
        beeper->notAllowed();
    }
}

void Extra::updateFouls(uint8_t fouls, bool force) {
    if (enabled || force) {
        this-> fouls = fouls;
        inputTimer.reset();
        foulsConfirmationTimer.stop();
        updating = true;
        updateFoulsDisplay();
    }
}

void Extra::resetTimeouts() {
    updateTimeouts(0, true);
}

void Extra::increaseTimeouts() {
    if (timeouts < maxTimeouts) {
        updateTimeouts(timeouts + 1);
    } else {
        beeper->notAllowed();
    }
}

void Extra::decreaseTimeouts() {
    if (timeouts > 0) {
        updateTimeouts(timeouts - 1);
    } else {
        beeper->notAllowed();
    }
}

void Extra::updateTimeouts(uint8_t timeouts, bool force) {
    if (enabled || force) {
        this->timeouts = timeouts;
        timeoutsConfirmationTimer.stop();
        updateTimeoutsDisplay();
    }
}

int Extra::decimalDigit(int value, int digit) {
    switch (digit) {
        case 0: return abs(value) % 10;
        case 1: return abs(value) / 10 % 10;
        case 2: return abs(value) / 100 % 10;
        default: return 0;
    }
}

void Extra::lastTwoMinutes() {
    if (timeouts == 0) {
        increaseTimeouts();
    }
}

void Extra::loopInput() {
    inputTimer.loop();
    if (updating) {
        if (inputTimer.isTriggered()) {
            if (fouls != prevFouls) {
                updateFoulsDisplay();
                if (onUpdateFouls != nullptr) {
                    onUpdateFouls(fouls);
                }
                foulsConfirmationTimer.reset();
                prevFouls = fouls;
                beeper->confirm();
            }
            updating = false;
        }
    }
    if (timeouts != prevTimeouts) {
        updateTimeoutsDisplay();
        if (onUpdateTimeouts != nullptr) {
            onUpdateTimeouts(timeouts);
        }
        prevTimeouts = timeouts;
        timeoutsConfirmationTimer.reset();
        beeper->confirm();
    }
}

void Extra::loopFoulsConfirmation() {
    foulsConfirmationTimer.loop();
    if (foulsConfirmationTimer.isRunning()) {
        updateFoulsDisplay(foulsConfirmationTimer.elapsed() / CONFIRMATION_FLASH_DURATION_MS % 2);
    }
}

void Extra::loopTimeoutsConfirmation() {
    timeoutsConfirmationTimer.loop();
    if (timeoutsConfirmationTimer.isRunning()) {
        updateTimeoutsDisplay(timeoutsConfirmationTimer.elapsed() / CONFIRMATION_FLASH_DURATION_MS % 2);
    }
}

void Extra::loop() {
    loopInput();
    loopFoulsConfirmation();
    loopTimeoutsConfirmation();
}

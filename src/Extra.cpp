#include "Extra.h"

Extra::Extra(
    LedControl *display, 
    uint8_t displayIndex,
    uint8_t brightness,
    bool invert,
    State *state,
    Beeper *beeper
) {
    this->display = display;
    this->displayIndex = displayIndex;
    this->brightness = brightness;
    this->invert = invert;
    this->state = state;
    this->beeper = beeper;
}

void Extra::setup(
    void (*onUpdateFouls)(uint8_t fouls), 
    void (*onUpdateTimeouts)(uint8_t timeouts, bool increased)
) {
    this->onUpdateFouls = onUpdateFouls;
    this->onUpdateTimeouts = onUpdateTimeouts;
    display->clearDisplay(displayIndex);
    display->setIntensity(displayIndex, brightness);
    display->shutdown(displayIndex, false);
    enable(false);
}

void Extra::reset() {
    updating = false;
    inputTimer.stop();
    resetFouls();
    resetTimeouts();
}

void Extra::resetPeriod() {
    display->clearDisplay(displayIndex);
    updating = false;
    inputTimer.stop();
    switch (state->getPhase()) {
        case TRAINING:
            resetFouls();
            resetTimeouts();
            break;
        case PREPARATION:
            break;
        case REGULAR_TIME:
            switch (state->getPeriod()) {
                case 1:
                    resetFouls();
                    resetTimeouts();
                    break;
                case 2:
                    resetFouls();
                    break;
                case 3:
                    resetFouls();
                    resetTimeouts();
                    break;
                case 4:
                    resetFouls();
                    break;
                default:
                    break;
            }
            break;
        case INTERVAL:
            break;
        case EXTRA_TIME:
            resetFouls();
            resetTimeouts();
            break;
        default:
            break;
    }
    maxTimeouts = state->getMaxTimeouts();
    updateFoulsDisplay(SHOW_FOULS);
    updateTimeoutsDisplay(SHOW_TIMEOUTS);
}

void Extra::enable(bool enabled) {
    this->enabled = enabled;
}

bool Extra::isEnabled() {
    return enabled;
}

void Extra::stateChange() {
    enable(state->isGamePeriod());
}

void Extra::printTimeoutChar(uint8_t pos, bool show) {
    if (show) {
        display->setChar(displayIndex, pos, 'o', false);
    } else {
        display->setChar(displayIndex, pos, '_', false);
    }
}

void Extra::updateFoulsDisplay(FoulsDisplayMode foulsDisplayMode) {
    if (enabled) {
        if (foulsDisplayMode == SHOW_BONUS) {
            display->setChar(displayIndex, FOULS_POS[invert][2], 'b', false);
            display->setChar(displayIndex, FOULS_POS[invert][1], 'o', false);
            display->setChar(displayIndex, FOULS_POS[invert][0], 'n', false);
        } else if (foulsDisplayMode == SHOW_FOULS) {
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
}

void Extra::publishFouls() {
    if (onUpdateFouls != nullptr) {
        onUpdateFouls(fouls);
    }
}

void Extra::updateTimeoutsDisplay(TimeoutsDisplayMode timeoutsDisplayMode) {
    if (enabled) {
        if (timeoutsDisplayMode == SHOW_TIMEOUTS && maxTimeouts > 2) {
            printTimeoutChar(TIMEOUTS_POS[invert][2], timeouts > 2);
        } else {
            display->setChar(displayIndex, TIMEOUTS_POS[invert][2], ' ', false);
        }
        if (timeoutsDisplayMode == SHOW_TIMEOUTS && maxTimeouts > 1) {
            printTimeoutChar(TIMEOUTS_POS[invert][1], timeouts > 1);
        } else {
            display->setChar(displayIndex, TIMEOUTS_POS[invert][1], ' ', false);
        }
        if (timeoutsDisplayMode == SHOW_TIMEOUTS && maxTimeouts > 0) {
            printTimeoutChar(TIMEOUTS_POS[invert][0], timeouts > 0);
        } else {
            display->setChar(displayIndex, TIMEOUTS_POS[invert][0], ' ', false);
        }
    }
}

void Extra::publishTimeouts(bool increased) {
    if (onUpdateTimeouts != nullptr) {
        onUpdateTimeouts(timeouts, increased);
    }
}

void Extra::resetFouls() {
    fouls = 0;
    inputTimer.stop(true);
    foulsConfirmationTimer.stop();
    bonusConfirmationTimer.stop();
    updating = true;
    updateFoulsDisplay(SHOW_FOULS);
    publishFouls();
}

void Extra::updateFouls(uint8_t fouls) {
    if (enabled) {
        this->fouls = fouls;
        inputTimer.reset();
        foulsConfirmationTimer.stop();
        bonusConfirmationTimer.stop();
        updating = true;
        updateFoulsDisplay(SHOW_FOULS);
    }
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

void Extra::resetTimeouts() {
    timeouts = 0;
    timeoutsConfirmationTimer.stop();
    updateTimeoutsDisplay(SHOW_TIMEOUTS);
    publishTimeouts();
}

void Extra::updateTimeouts(uint8_t timeouts) {
    if (enabled) {
        this->timeouts = timeouts;
        timeoutsConfirmationTimer.stop();
        updateTimeoutsDisplay(SHOW_TIMEOUTS);
    }
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
                updateFoulsDisplay(SHOW_FOULS);
                publishFouls();
                foulsConfirmationTimer.reset();
                prevFouls = fouls;
                beeper->confirm();
            }
            updating = false;
        }
    }
    if (timeouts != prevTimeouts) {
        updateTimeoutsDisplay(SHOW_TIMEOUTS);
        publishTimeouts(timeouts > prevTimeouts);
        prevTimeouts = timeouts;
        timeoutsConfirmationTimer.reset();
        beeper->confirm();
    }
}

void Extra::loopFoulsConfirmation() {
    foulsConfirmationTimer.loop();
    if (foulsConfirmationTimer.isRunning()) {
        updateFoulsDisplay((foulsConfirmationTimer.elapsed() / CONFIRMATION_FLASH_DURATION_MS % 2) ? SHOW_FOULS : FOULS_OFF);
    } else if (fouls == 4 && foulsConfirmationTimer.isTriggered()) {
        bonusConfirmationTimer.reset();
    }
}

void Extra::loopBonusConfirmation() {
    bonusConfirmationTimer.loop();
    if (bonusConfirmationTimer.isRunning()) {
        updateFoulsDisplay((bonusConfirmationTimer.elapsed() / BONUS_FLASH_DURATION_MS % 2) ? SHOW_BONUS : FOULS_OFF);
    } else if (bonusConfirmationTimer.isTriggered()) {
        updateFoulsDisplay(SHOW_FOULS);
    }
}

void Extra::loopTimeoutsConfirmation() {
    timeoutsConfirmationTimer.loop();
    if (timeoutsConfirmationTimer.isRunning()) {
        updateTimeoutsDisplay((timeoutsConfirmationTimer.elapsed() / CONFIRMATION_FLASH_DURATION_MS % 2) ? SHOW_TIMEOUTS : TIMEOUTS_OFF);
    }
}

void Extra::loop() {
    loopInput();
    loopFoulsConfirmation();
    loopBonusConfirmation();
    loopTimeoutsConfirmation();
}

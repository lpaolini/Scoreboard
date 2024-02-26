#include "Score.h"

Score::Score(
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

void Score::setup(void (*onUpdate)(uint8_t score)) {
    this->onUpdate = onUpdate;
    display->shutdown(displayIndex, true);
    display->clearDisplay(displayIndex);
    display->setIntensity(displayIndex, brightness);
    reset();
}

void Score::reset() {
    score = 0;
    delta = 0;
    prevDelta = 0;
    updating = false;
    inputTimer.stop();
    flashTimer.stop();
    updateScoreDisplay();
    updateDeltaDisplay(delta, false);
}

void Score::enable(bool enabled) {
    display->shutdown(displayIndex, !enabled);
    this->enabled = enabled;
}

void Score::stateChange() {
    if (!state->isGameMode()) {
        enable(false);
    }
}

void Score::resetPeriod() {
    if (state->isStartOfGame()) {
        reset();
    }
    enable(true);
}

void Score::startTimer() {
    inputTimer.reset();
}

void Score::updateScoreDisplay(bool show) {
    if (show) {
        uint8_t hundreds = decimalDigit(score, 2);
        if (hundreds || THREE_DIGIT_SCORE) {
            display->setDigit(displayIndex, SCORE_POS[invert][2], hundreds, false);
            display->setDigit(displayIndex, SCORE_POS[invert][1], decimalDigit(score, 1), false);
            display->setDigit(displayIndex, SCORE_POS[invert][0], decimalDigit(score, 0), false);
        } else {
            if (invert) {
                display->setChar(displayIndex, SCORE_POS[invert][2], ' ', false);
                display->setDigit(displayIndex, SCORE_POS[invert][1], decimalDigit(score, 1), false);
                display->setDigit(displayIndex, SCORE_POS[invert][0], decimalDigit(score, 0), false);
            } else {
                display->setDigit(displayIndex, SCORE_POS[invert][2], decimalDigit(score, 1), false);
                display->setDigit(displayIndex, SCORE_POS[invert][1], decimalDigit(score, 0), false);
                display->setChar(displayIndex, SCORE_POS[invert][0], ' ', false);
            }
        }
    } else {
        display->setChar(displayIndex, SCORE_POS[invert][2], ' ', false);
        display->setChar(displayIndex, SCORE_POS[invert][1], ' ', false);
        display->setChar(displayIndex, SCORE_POS[invert][0], ' ', false);
    }
}

void Score::updateDeltaDisplay(int8_t delta, bool showIndicator) {
    if (showIndicator) {
        display->setChar(displayIndex, DELTA_POS[invert][0], 'P', false);
        display->setDigit(displayIndex, DELTA_POS[invert][1], decimalDigit(delta, 0), false);
    } else {
        display->setChar(displayIndex, DELTA_POS[invert][0], ' ', false);
        if (delta != 0) {
            display->setDigit(displayIndex, DELTA_POS[invert][1], decimalDigit(delta, 0), false);
        } else {
            display->setChar(displayIndex, DELTA_POS[invert][1], ' ', false);
        }
    }
    display->setChar(displayIndex, DELTA_POS[invert][2], delta < 0 ? '-' : ' ', false);
}

void Score::clearDelta() {
    delta = 0;
    display->setChar(displayIndex, DELTA_POS[invert][0], ' ', false);
    display->setChar(displayIndex, DELTA_POS[invert][1], ' ', false);
    display->setChar(displayIndex, DELTA_POS[invert][2], ' ', false);
}

void Score::undo() {
    if (prevDelta > 0) {
        if (delta == 0) {
            delta = -prevDelta;
        } else {
            delta = 0;
        }
        inputTimer.stop();
        updating = true;
        updateDeltaDisplay(delta);
    } else {
        beeper->notAllowed();
    }
}

int Score::decimalDigit(int value, int digit) {
    switch (digit) {
        case 0: return abs(value) % 10;
        case 1: return abs(value) / 10 % 10;
        case 2: return abs(value) / 100 % 10;
        default: return 0;
    }
}

void Score::increaseDelta(bool roll) {
    if (enabled) {
        delta = roll
            ? (max(0, delta) + 1) % 4
            : min(delta + 1, 3);
        inputTimer.stop();
        updating = true;
        updateDeltaDisplay(delta);
    }
}

void Score::decreaseDelta(bool roll) {
    if (enabled) {
        delta = roll
            ? (min(0, delta) - 1) % 4
            : max(delta - 1, -3);
        inputTimer.stop();
        updating = true;
        updateDeltaDisplay(delta);
    }
}

void Score::increaseScore() {
    if (enabled) {
        score = limitScore(score + 1);
        prevDelta = 0;
        updateScoreDisplay();
        clearDelta();
        onUpdate(score);
    }
}

void Score::decreaseScore() {
    if (enabled) {
        score = limitScore(score - 1);
        prevDelta = 0;
        updateScoreDisplay();
        clearDelta();
        onUpdate(score);
    }
}

uint8_t Score::limitScore(int16_t score) {
    return min(MAX_SCORE, max(0, score));
}

void Score::loopInput() {
    inputTimer.loop();
    if (updating) {
        if (inputTimer.isTriggered()) {
            if (delta != 0) {
                score = limitScore(score + delta);
                prevDelta = delta;
                delta = 0;
                updateScoreDisplay();
                onUpdate(score);
                flashTimer.reset();
                beeper->confirm();
            }
            updateDeltaDisplay(prevDelta, false);
            updating = false;
        }
    }
}

void Score::loopFlash() {
    flashTimer.loop();
    if (flashTimer.isRunning()) {
        updateScoreDisplay(flashTimer.elapsed() / CONFIRMATION_FLASH_DURATION_MS % 2);
    }
}

void Score::loop() {
    loopInput();
    loopFlash();
}

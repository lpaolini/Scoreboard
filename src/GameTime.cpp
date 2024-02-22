#include "GameTime.h"

GameTime::GameTime(
    Adafruit_7segment *display,
    uint8_t address,
    uint8_t brightness,
    State *state,
    Beeper *beeper
) {
    this->display = display;
    this->address = address;
    this->brightness = brightness;
    this->state = state;
    this->beeper = beeper;
}

void GameTime::setup( 
    void (*onTimeUpdate)(unsigned long time),
    void (*onGameMode)(bool gameMode),
    void (*onResetPeriod)(uint8_t period),
    void (*onLastTwoMinutes)()
) {
    this->onTimeUpdate = onTimeUpdate;
    this->onGameMode = onGameMode;
    this->onResetPeriod = onResetPeriod;
    this->onLastTwoMinutes = onLastTwoMinutes;
    display->begin(address);
    setBrightness(brightness);
    reset();
}

void GameTime::setBrightness(uint8_t brightness) {
    display->setBrightness(brightness);
}

uint8_t GameTime::presetCount() {
    return sizeof(preset)/sizeof(preset[0]);
}

void GameTime::reset() {
    state->setMode(SET_STEP);
    state->setPhase(REGULAR_TIME);
    state->setPeriod(1);
    homeScore = 0;
    guestScore = 0;
    currentPreset = defaultPreset;
    resetPeriod();
}

void GameTime::resetPeriod(bool advancePeriod) {
    if (state->getPhase() != END_OF_GAME) {
        if (advancePeriod) {
            increaseStep();
        }
        onGameMode(false);
        state->setMode(SET_STEP);
        time = preset[currentPreset];
    }
}

bool GameTime::isRunning() {
    return state->getMode() == RUN;
}

bool GameTime::isEndOfPeriod() {
    return state->getMode() == STOP && time == 0;
}

int GameTime::decimalDigit(int value, int digit) {
    switch (digit) {
        case 0: return abs(value) % 10;
        case 1: return abs(value) / 10 % 10;
        case 2: return abs(value) / 100 % 10;
        default: return 0;
    }
}

void GameTime::showTime() {
    display->setDisplayState(true);

    showLastTwoMinutesAlert(time);

    if ((state->getPhase() == REGULAR_TIME || state->getPhase() == EXTRA_TIME) && time <= 59900) {
        showSecTenth(time);
    } else {
        showMinSec(time);
    }

    if ((state->getMode() == RUN || state->getMode() == STOP) && lastTime != time) {
        if (state->getMode() == RUN && state->getPhase() == REGULAR_TIME && state->getPeriod() == 4 && lastTime > 120000 && time <= 120000) {
            onLastTwoMinutes();
        }
        lastTime = time;
        onTimeUpdate(time);
    }
}

void GameTime::showMinSec(unsigned long time) {
    unsigned long adjustedTime = time + 999; // adjust for truncation to whole seconds
    uint8_t min = adjustedTime / 60000;
    uint8_t sec = adjustedTime % 60000 / 1000;
    if (min >= 10) {
        display->writeDigitNum(0, decimalDigit(min, 1), false);
    }
    display->writeDigitNum(1, decimalDigit(min, 0), false);
    display->writeDigitNum(3, decimalDigit(sec, 1), false);
    display->writeDigitNum(4, decimalDigit(sec, 0), false);
    display->drawColon(state->getMode() == RUN ? (time) / 150 % 2 : true);
    display->writeDisplay();
}

void GameTime::showSecTenth(unsigned long time) {
    unsigned long adjustedTime = time + 99; // adjust for truncation to tenths of seconds
    uint8_t sec = adjustedTime / 1000;
    uint8_t tenth = adjustedTime % 1000 / 100;

    if (sec < 10) {
        display->writeDigitAscii(1, ' ', false);
    } else {
        display->writeDigitNum(1, decimalDigit(sec, 1), false);
    }
    display->writeDigitNum(3, decimalDigit(sec, 0), true);
    display->writeDigitNum(4, decimalDigit(tenth, 0), false);
    display->drawColon(false);
    display->writeDisplay();
}

void GameTime::showLastTwoMinutesAlert(unsigned long time) {
    if (state->getMode() == RUN && state->getPhase() == REGULAR_TIME && state->getPeriod() == 4 && time <= 120000 && time / 250 % 2) {
        display->writeDigitRaw(0, 0b01001001);
    } else {
        display->writeDigitRaw(0, 0b00000000);
    }
}

void GameTime::showPeriod() {
    display->setDisplayState(true);

    switch (state->getPhase()) {
        case PREPARATION:
            display->writeDigitAscii(0, 'P', false);
            display->writeDigitAscii(1, 'r', false);
            display->writeDigitAscii(3, 'E', false);
            display->writeDigitAscii(4, 'P', false);
            break;
        case REGULAR_TIME:
            display->writeDigitAscii(0, ' ', false);
            display->writeDigitAscii(1, 'P', false);
            display->writeDigitNum(3, state->getPeriod(), false);
            display->writeDigitAscii(4, ' ', false);
            break;
        case INTERVAL:
            display->writeDigitAscii(0, 'I', false);
            display->writeDigitAscii(1, 'n', false);
            display->writeDigitAscii(3, 't', false);
            display->writeDigitAscii(4, 'r', false);
            break;
        case EXTRA_TIME:
            display->writeDigitAscii(0, 'S', false);
            display->writeDigitAscii(1, 'u', false);
            display->writeDigitAscii(3, 'p', false);
            display->writeDigitAscii(4, 'p', false);
            break;
        case END_OF_GAME:
            display->writeDigitAscii(0, '-', false);
            display->writeDigitAscii(1, '-', false);
            display->writeDigitAscii(3, '-', false);
            display->writeDigitAscii(4, '-', false);
            break;
        default:
            break;
    }

    display->drawColon(false);
    display->writeDisplay();
}

void GameTime::setTime() {
    if (state->getPhase() != END_OF_GAME) {
        state->setMode(SET_TIME);
    }
}

void GameTime::start() {
    if (time == 0) {
        resetPeriod(true);
    } else {
        state->setMode(RUN);
        this->timeStart = millis();
        showTime();
        setBrightness(START_FLASH_BRIGHTNESS);
        startFlash.reset();
        beeper->timeStart();
    }
}

void GameTime::stop() {
    state->setMode(STOP);
    this->timeStop = millis();
    showTime();
    beeper->timeStop();
}

void GameTime::next() {
    switch (state->getMode()) {
        case SET_STEP:
            setTime();
            break;
        case SET_TIME:
            switch (state->getPhase()) {
                case PREPARATION:
                    onResetPeriod(0);
                    break;
                case REGULAR_TIME:
                    onResetPeriod(state->getPeriod());
                    break;
                case INTERVAL:
                    onResetPeriod(6);
                    break;
                case EXTRA_TIME:
                    onResetPeriod(5);
                    break;
                default:
                    break;
            }
            stop();
            break;
        case RUN:
            stop();
            break;
        case STOP:
            start();
            break;
        default:
            break;
    }
}

void GameTime::prev() {
    switch (state->getMode()) {
        case SET_TIME:
            state->setMode(SET_STEP);
            break;
        case RUN:
            stop();
            break;
        case STOP:
            start();
            break;
        default:
            break;
    }
}

void GameTime::increaseRemainingTime() {
    time = min(MAX_TIME, time + 1000);
    hold.reset();
    showTime();
}

void GameTime::increaseStep() {
    switch (state->getPhase()) {
        case PREPARATION:
            state->setPhase(REGULAR_TIME);
            state->setPeriod(1);
            break;
        case REGULAR_TIME:
            increasePeriod();
            break;
        case INTERVAL:
            state->setPhase(REGULAR_TIME);
            state->setPeriod(3);
            break;
        case EXTRA_TIME:
            if (!isParity()) {
                state->setPhase(END_OF_GAME);
            }
            break;
        default:
            break;
    }
}

void GameTime::increasePeriod() {
    switch (state->getPeriod()) {
        case 1:
            state->setPeriod(2);
            break;
        case 2:
            state->setPhase(INTERVAL);
            break;
        case 3:
            state->setPeriod(4);
            break;
        case 4:
            if (isParity()) {
                state->setPhase(EXTRA_TIME);
            } else {
                state->setPhase(END_OF_GAME);
            }
            break;
        default:
            break;
    }
}

void GameTime::increaseTime() {
    currentPreset = min(currentPreset + 1, presetCount() - 1);
    time = preset[currentPreset];
}

void GameTime::increase() {
    switch (state->getMode()) {
        case STOP: increaseRemainingTime(); break;
        case SET_STEP: increaseStep(); break;
        case SET_TIME: increaseTime(); break;
        default: break;
    }
}

void GameTime::decreaseStep() {
    switch (state->getPhase()) {
        case END_OF_GAME:
            if (isParity()) {
                state->setPhase(EXTRA_TIME);
            } else {
                state->setPhase(REGULAR_TIME);
                state->setPeriod(4);
            }
            break;
        case EXTRA_TIME:
            state->setPhase(REGULAR_TIME);
            state->setPeriod(4);
            break;
        case INTERVAL:
            state->setPhase(REGULAR_TIME);
            state->setPeriod(2);
            break;
        case REGULAR_TIME:
            decreasePeriod();
            break;
        default:
            break;
    }
}

void GameTime::decreasePeriod() {
    switch (state->getPeriod()) {
        case 1:
            state->setPhase(PREPARATION);
            break;
        case 2:
            state->setPeriod(1);
            break;
        case 3:
            state->setPhase(INTERVAL);
            break;
        case 4:
            state->setPeriod(3);
            break;
        default:
            break;
    }
}

void GameTime::decreaseTime() {
    currentPreset = max(currentPreset - 1, 0);
    time = preset[currentPreset];
}

void GameTime::decreaseRemainingTime() {
    if (time > 0) {
        time = max(1000L, time - min(1000, time));
        hold.reset();
        showTime();
    }
}

void GameTime::decrease() {
    switch (state->getMode()) {
        case STOP: decreaseRemainingTime(); break;
        case SET_STEP: decreaseStep(); break;
        case SET_TIME: decreaseTime(); break;
        default: break;
    }
}

void GameTime::setHomeScore(uint8_t homeScore) {
    this->homeScore = homeScore; 
}

void GameTime::setGuestScore(uint8_t guestScore) {
    this->guestScore = guestScore;
}

void GameTime::loopStop() {
    bool show = (hold.isRunning() && !hold.isTriggered()) || (millis() - timeStop) / STOP_FLASH_DURATION_MS % 2;
    display->setDisplayState(show);
}

bool GameTime::isParity() {
    return homeScore > 0 && guestScore > 0 && homeScore == guestScore;
}

void GameTime::loopRun() {
    unsigned long now = millis();
    unsigned long elapsed = now - timeStart;
    time = time > elapsed ? time - elapsed : 0;
    timeStart = now;
    if (time > 0) {
        showTime();
    } else {
        stop();
    }
}

void GameTime::loopSetTime() {
    showTime();
}

void GameTime::loopSetStep() {
    showPeriod();
}

void GameTime::loop() {
    hold.loop();
    startFlash.loop();
    if (startFlash.isExpired()) {
        setBrightness(brightness);
    }
    switch (state->getMode()) {
        case STOP: loopStop(); break;
        case RUN: loopRun(); break;
        case SET_TIME: loopSetTime(); break;
        case SET_STEP: loopSetStep(); break;
    }
}

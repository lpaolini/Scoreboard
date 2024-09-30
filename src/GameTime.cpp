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
    void (*onTimeUpdate)(Time time, bool tenths),
    void (*onResetPeriod)(),
    void (*onLastTwoMinutes)(),
    void (*onEndOfPeriod)(),
    void (*onEndOfTimeout)()
) {
    this->onTimeUpdate = onTimeUpdate;
    this->onResetPeriod = onResetPeriod;
    this->onLastTwoMinutes = onLastTwoMinutes;
    this->onEndOfPeriod = onEndOfPeriod;
    this->onEndOfTimeout = onEndOfTimeout;
    display->begin(address);
    setBrightness(brightness);
    enable(false);
}

void GameTime::setBrightness(uint8_t brightness) {
    display->setBrightness(brightness);
}

uint8_t GameTime::presetCount() {
    return sizeof(preset)/sizeof(preset[0]);
}

void GameTime::reset() {
    state->reset();
    homeScore = 0;
    guestScore = 0;
    currentPreset = defaultPreset;
    resetPeriod();
}

void GameTime::resetPeriod(bool advancePeriod) {
    if (isEndOfGame()) {
        beeper->notAllowed();
    } else {
        state->setMode(SET_STEP);
        if (advancePeriod && state->getPhase() != TRAINING) {
            increaseStep();
        }
        periodTime = preset[currentPreset];
        beeper->confirm();
    }
}

void GameTime::stateChange() {
    enable(state->getMode() != RESET);
}

void GameTime::enable(bool enabled) {
    display->setDisplayState(enabled);
    display->writeDisplay();
}

int GameTime::decimalDigit(int value, int digit) {
    switch (digit) {
        case 0: return abs(value) % 10;
        case 1: return abs(value) / 10 % 10;
        case 2: return abs(value) / 100 % 10;
        default: return 0;
    }
}

void GameTime::showPeriodTime() {
    display->setDisplayState(true);

    if (periodTime <= 59900) {
        adjustedTime = periodTime + 99; // adjust for truncation to tenths of seconds
        current.fields.min = 0;
        current.fields.sec = adjustedTime / 1000;
        current.fields.tenth = adjustedTime % 1000 / 100;
        tenths = current.time > 0;
        showPeriodTimeSecTenth();
    } else {
        adjustedTime = periodTime + 999; // adjust for truncation to whole seconds
        current.fields.min = adjustedTime / 60000;
        current.fields.sec = adjustedTime % 60000 / 1000;
        current.fields.tenth = 0;
        tenths = false;
        showPeriodTimeMinSec();
    }

    if (state->isGameMode()) {
        if (last.time != current.time) {
            if (state->getChrono() == RUN && current.fields.min == 2 && current.fields.sec == 0 && current.time < last.time) {
                if (state->isFourthPeriod()) {
                    onLastTwoMinutes();
                }
                if (state->isFourthPeriodOrOvertime()) {
                    beeper->confirm();
                }
            }
            last.time = current.time;
            publishTime();
        }
    }
}

void GameTime::showPeriodTimeMinSec() {
    if (current.fields.min >= 10) {
        display->writeDigitNum(0, decimalDigit(current.fields.min, 1), false);
    } else {
        showLastTwoMinutesAlert();
    }
    display->writeDigitNum(1, decimalDigit(current.fields.min, 0), false);
    display->writeDigitNum(3, decimalDigit(current.fields.sec, 1), false);
    display->writeDigitNum(4, decimalDigit(current.fields.sec, 0), false);
    display->drawColon(state->getChrono() == RUN ? (periodTime) / RUN_COLON_FLASH_DURATION_MS % 2 : true);
    display->writeDisplay();
}

void GameTime::showPeriodTimeSecTenth() {
    showLastTwoMinutesAlert();
    if (current.fields.sec >= 10) {
        display->writeDigitNum(1, decimalDigit(current.fields.sec, 1), false);
    } else {
        display->writeDigitAscii(1, ' ', false);
    }
    display->writeDigitNum(3, decimalDigit(current.fields.sec, 0), true);
    display->writeDigitNum(4, decimalDigit(current.fields.tenth, 0), false);
    display->drawColon(false);
    display->writeDisplay();
}

void GameTime::showLastTwoMinutesAlert() {
    if (state->isFourthPeriodOrOvertime() && periodTime <= 120000 && periodTime / 250 % 2 && state->getChrono() == RUN) {
        display->writeDigitRaw(0, 0b01001001); // three horizontal bars
    } else {
        display->writeDigitAscii(0, ' ', false);
    }
}

void GameTime::showTimeoutTime() {
    display->setDisplayState(true);
    adjustedTime = timeoutTime + 999; // adjust for truncation to whole seconds
    uint8_t seconds = adjustedTime / 1000;
    display->writeDigitAscii(0, '-', false);
    display->writeDigitNum(1, decimalDigit(seconds, 1), false);
    display->writeDigitNum(3, decimalDigit(seconds, 0), false);
    display->writeDigitAscii(4, '-', false);
    display->drawColon(false);
    display->writeDisplay();
}

void GameTime::showPeriod() {
    display->setDisplayState(true);
    switch (state->getPhase()) {
        case TRAINING:
            display->writeDigitAscii(0, 'A', false);
            display->writeDigitAscii(1, 'L', false);
            display->writeDigitAscii(3, 'L', false);
            display->writeDigitAscii(4, 'n', false);
            break;
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
        default:
            break;
    }

    display->drawColon(false);
    display->writeDisplay();
}

void GameTime::start() {
    state->setChrono(RUN);
    this->timeStart = millis();
    showPeriodTime();
    setBrightness(START_FLASH_BRIGHTNESS);
    startFlash.reset();
    beeper->timeStart();
}

void GameTime::stop() {
    state->setChrono(STOP);
    this->timeStop = millis();
    beeper->timeStop();
}

void GameTime::next() {
    switch (state->getMode()) {
        case SET_STEP:
            state->setMode(SET_TIME);
            break;
        case SET_TIME:
            last.time = 0;
            state->setMode(GAME);
            stop();
            onResetPeriod();
            break;
        case GAME:
            if (state->getChrono() == STOP) {
                if (periodTime == 0) {
                    resetPeriod(true);
                } else {
                    start();
                }
            } else {
                stop();
            }
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
        case GAME:
            if (state->getChrono() == STOP) {
                if (periodTime == preset[currentPreset]) {
                    resetPeriod(false);
                }
            }
            break;
        default:
            break;
    }
}

void GameTime::increaseRemainingTime() {
    periodTime = min(MAX_TIME, periodTime + 1000);
    hold.reset();
    showPeriodTime();
}

void GameTime::increaseStep() {
    switch (state->getPhase()) {
        case TRAINING:
            state->setPhase(PREPARATION);
            break;
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
            }
            break;
        default:
            break;
    }
}

void GameTime::increaseTime() {
    currentPreset = min(currentPreset + 1, presetCount() - 1);
    periodTime = preset[currentPreset];
}

void GameTime::increase() {
    switch (state->getMode()) {
        case GAME: 
            if (state->getChrono() == STOP) {
                increaseRemainingTime();
            }
            break;
        case SET_STEP:
            increaseStep();
            break;
        case SET_TIME:
            increaseTime();
            break;
        default:
            break;
    }
}

void GameTime::decreaseStep() {
    switch (state->getPhase()) {
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
        case PREPARATION:
            state->setPhase(TRAINING);
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
    periodTime = preset[currentPreset];
}

void GameTime::decreaseRemainingTime() {
    if (periodTime > 0) {
        periodTime = max(1000L, periodTime - min(1000, periodTime));
        hold.reset();
        showPeriodTime();
    }
}

void GameTime::decrease() {
    switch (state->getMode()) {
        case GAME:
            if (state->getChrono() == STOP) {
                decreaseRemainingTime();
            }
            break;
        case SET_STEP:
            decreaseStep();
            break;
        case SET_TIME:
            decreaseTime();
            break;
        default:
            break;
    }
}

void GameTime::setHomeScore(uint8_t homeScore) {
    this->homeScore = homeScore; 
}

void GameTime::setGuestScore(uint8_t guestScore) {
    this->guestScore = guestScore;
}

void GameTime::startTimeout() {
    state->setChrono(TIMEOUT);
    this->timeStart = millis();
    timeoutTime = TIMEOUT_TIME;
}

void GameTime::publishTime() {
    if (onTimeUpdate != nullptr) {
        onTimeUpdate(current, tenths);
    }
}

bool GameTime::isParity() {
    return homeScore == guestScore;
}

bool GameTime::isEndOfPeriod() {
    return state->isGameMode() && state->getChrono() == STOP && current.time == 0;
}

bool GameTime::isEndOfGame() {
    return state->isFourthPeriodOrOvertime() && state->getChrono() == STOP && !isParity() && current.time == 0;
}

void GameTime::loopStop() {
    bool show = (hold.isRunning() && !hold.isTriggered()) || (millis() - timeStop) / STOP_FLASH_DURATION_MS % 2;
    if (show) {
        showPeriodTime();
    } else {
        display->setDisplayState(false);
    }
}

void GameTime::loopRun() {
    unsigned long now = millis();
    unsigned long elapsed = now - timeStart;
    periodTime = periodTime > elapsed ? periodTime - elapsed : 0;
    timeStart = now;
    if (periodTime > 0) {
        showPeriodTime();
    } else {
        stop();
        if (onEndOfPeriod != nullptr) {
            onEndOfPeriod();
        }
    }
}

void GameTime::loopTimeout() {
    unsigned long now = millis();
    unsigned long elapsed = now - timeStart;
    timeoutTime = timeoutTime > elapsed ? timeoutTime - elapsed : 0;
    timeStart = now;
    if (timeoutTime > 0) {
        showTimeoutTime();
    } else {
        stop();
        if (onEndOfTimeout != nullptr) {
            onEndOfTimeout();
        }
    }
}

void GameTime::loopGame() {
    switch (state->getChrono()) {
        case RUN:
            loopRun();
            break;
        case TIMEOUT:
            loopTimeout();
            break;
        case STOP:
            loopStop();
            break;
        default:
            break;
    }
}

void GameTime::loopSetTime() {
    showPeriodTime();
}

void GameTime::loopSetStep() {
    showPeriod();
}

void GameTime::loop() {
    hold.loop();
    startFlash.loop();
    if (startFlash.isTriggered()) {
        setBrightness(brightness);
    }
    switch (state->getMode()) {
        case GAME:
            loopGame();
            break;
        case SET_TIME:
            loopSetTime();
            break;
        case SET_STEP:
            loopSetStep();
            break;
        default: 
            break;
    }
}

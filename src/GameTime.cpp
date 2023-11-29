#include "GameTime.h"

GameTime::GameTime(
    Adafruit_7segment *display,
    uint8_t address,
    uint8_t brightness,
    void (*onUpdate)(unsigned long time)
) {
    this->display = display;
    this->address = address;
    this->brightness = brightness;
    this->onUpdate = onUpdate;
}

void GameTime::setup() {
    display->begin(address);
    display->setBrightness(brightness);
    reset();
}

void GameTime::reset() {
    if (!isRunning()) {
        currentPreset = defaultPreset;
        resetPeriod();
    }
}

void GameTime::resetPeriod() {
    if (!isRunning()) {
        mode = SET_TIME;
        time = preset[currentPreset];
    }
}

bool GameTime::isRunning() {
    return mode == RUN;
}

bool GameTime::isEndOfPeriod() {
    return mode == STOP && time == 0;
}

int GameTime::decimalDigit(int value, int digit) {
    switch (digit) {
        case 0: return abs(value) % 10;
        case 1: return abs(value) / 10 % 10;
        case 2: return abs(value) / 100 % 10;
        default: return 0;
    }
}

uint8_t GameTime::presetCount() {
    return sizeof(preset)/sizeof(preset[0]);
}

void GameTime::showMinSec(unsigned long time, bool showColon) {
    uint8_t min = time / 60000;
    uint8_t sec = time % 60000 / 1000;
    if (min < 10) {
        display->writeDigitAscii(0, ' ', false);
    } else {
        display->writeDigitNum(0, decimalDigit(min, 1), false);
    }
    display->writeDigitNum(1, decimalDigit(min, 0), false);
    display->writeDigitNum(3, decimalDigit(sec, 1), false);
    display->writeDigitNum(4, decimalDigit(sec, 0), false);
    display->drawColon(showColon);
    display->writeDisplay();
}

void GameTime::showSecTenth(unsigned long time) {
    uint8_t sec = time / 1000;
    uint8_t tenth = time % 1000 / 100;

    display->writeDigitAscii(0, ' ', false);
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

void GameTime::update() {
    unsigned long now = millis();

    if (time < 59900) {
        showSecTenth(time + 99);
    } else {
        bool showColon = mode == RUN
            ? (now - timeStop) / 250 % 2
            : true;
        showMinSec(time + 999, showColon);
    }

    if (lastTime != time) {
        lastTime = time;
        onUpdate(time);
    }
}

void GameTime::start() {
    if (time > 0) {
        mode = RUN;
        this->timeStart = millis();
        update();
    }
}

void GameTime::stop() {
    mode = STOP;
    this->timeStop = millis();
    update();
}

void GameTime::toggle() {
    if (mode == RUN) {
        stop();
    } else {
        start();
    }
}

void GameTime::increaseStop(uint8_t delta) {
    time = min(MAX_TIME, time + 1000 * delta);
    hold.reset();
    update();
}

void GameTime::increaseSetTime() {
    currentPreset = min(currentPreset + 1, presetCount() - 1);
    time = preset[currentPreset];
}

void GameTime::increase(uint8_t delta) {
    switch (mode) {
        case STOP: increaseStop(delta); break;
        case SET_TIME: increaseSetTime(); break;
        default: break;
    }
}

void GameTime::decreaseSetTime() {
    currentPreset = max(currentPreset - 1, 0);
    time = preset[currentPreset];
}

void GameTime::decreaseStop(uint8_t delta) {
    if (time > 0) {
        time = max(0L, time - 1000 * delta);
        hold.reset();
        update();
    }
}

void GameTime::decrease(uint8_t delta) {
    switch (mode) {
        case STOP: decreaseStop(delta); break;
        case SET_TIME: decreaseSetTime(); break;
        default: break;
    }
}

void GameTime::loopStop() {
    unsigned long now = millis();
    bool show = (hold.isRunning() && !hold.isTriggered()) || (now - timeStop) / 200 % 2;
    if (show) {
        update();
    } else {
        display->clear();
        display->writeDisplay();
    }
}

void GameTime::loopRun() {
    unsigned long now = millis();
    unsigned long elapsed = now - timeStart;
    time = time > elapsed ? time - elapsed : 0;
    timeStart = now;
    if (time > 0) {
        update();
    } else {
        stop();
    }
}

void GameTime::loopSetTime() {
    if (time == 0) {
        // time = TIME_10_MIN;
    }
    update();
}

void GameTime::loop() {
    hold.loop();
    switch (mode) {
        case STOP: loopStop(); break;
        case RUN: loopRun(); break;
        case SET_TIME: loopSetTime(); break;
    }
}

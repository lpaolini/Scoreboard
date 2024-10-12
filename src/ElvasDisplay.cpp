#include "ElvasDisplay.h"

ElvasDisplay::ElvasDisplay(uint8_t outputPin, uint8_t ledPin, State *state) {
    this->outputPin = outputPin;
    this->ledPin = ledPin;
    this->state = state;
    init();
}

void ElvasDisplay::setup() {
    pinMode(outputPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
}

void ElvasDisplay::reset() {
    enabled = false;
    for (uint8_t i = 0; i < DATA_LENGTH; i++) {
        currentState.data[i] = 0;
    }
    nextBit = 0;
    setUnknown1(true);
    setUnknown2(true);
    copyState(nextState.data, currentState.data);
    showPeriod = false;
    updateRequired = true;
}

void ElvasDisplay::stateChange() {
    enabled = true;
    if (state->isGamePeriod()) {
        showFouls = true;
        showTimeouts = true;
    } else {
        showFouls = false;
        showTimeouts = false;
    }
    lastTimeChanged = millis();
}

int ElvasDisplay::decimalDigit(int value, int digit) {
    switch (digit) {
        case 0: return abs(value) % 10;
        case 1: return abs(value) / 10 % 10;
        case 2: return abs(value) / 100 % 10;
        default: return 0;
    }
}

void ElvasDisplay::setOutput(bool level) {
    digitalWrite(outputPin, level);
}

void ElvasDisplay::copyState(void *dst, const void *src) {
    noInterrupts();
    memcpy(dst, src, DATA_LENGTH * sizeof(uint8_t));
    interrupts();
}

void ElvasDisplay::check() {
    copyState(nextState.data, currentState.data);
    alterTimeDisplay();
    alterFoulsDisplay();
    alterTimeoutDisplay();
    updateRequired = memcmp(nextState.data, updateState.data, DATA_LENGTH * sizeof(uint8_t)) != 0;
}

void ElvasDisplay::alterTimeDisplay() {
    if (state->isGameMode()) {
        if (showPeriod) {
            nextState.fields.time3 = DIGIT_OFF;
            nextState.fields.time2 = DIGIT_OFF;
            nextState.fields.time1 = state->getPeriod();
            nextState.fields.time0 = DIGIT_OFF;
        }
        if (!showTime) {
            nextState.fields.time3 = DIGIT_OFF;
            nextState.fields.time2 = DIGIT_OFF;
            nextState.fields.time1 = DIGIT_OFF;
            nextState.fields.time0 = DIGIT_OFF;
        }
    } else {
        nextState.fields.time3 = DIGIT_OFF;
        nextState.fields.time2 = DIGIT_OFF;
        nextState.fields.time1 = DIGIT_OFF;
        nextState.fields.time0 = DIGIT_OFF;
    }
}

void ElvasDisplay::alterFoulsDisplay() {
    if (!showFouls) {
        nextState.fields.homeFouls1 = 0;
        nextState.fields.homeFouls0 = DIGIT_OFF;
        nextState.fields.guestFouls1 = 0;
        nextState.fields.guestFouls0 = DIGIT_OFF;
    }
}

void ElvasDisplay::alterTimeoutDisplay() {
    if (!showTimeouts) {
        nextState.fields.homeTimeouts = 0;
        nextState.fields.homeService = 0;
        nextState.fields.guestTimeouts = 0;
        nextState.fields.guestService = 0;
    }
}

void ElvasDisplay::update() {
    if (enabled) {
        if (updateRequired || nextBit != 0) {
            if (nextBit == 0) {
                // create data snapshop and re-arm trigger
                copyState(updateState.data, nextState.data);
                updateRequired = false;
                digitalWrite(ledPin, true);
                #ifdef ELVAS_DEBUG
                    Serial.print(F("\nElvas: ["));
                #endif
            }

            if (nextBit < START_LENGTH) {
                // head
                setOutput(HIGH);
            } else if (nextBit < START_LENGTH + 8 * DATA_LENGTH) {
                // data
                uint8_t dataBit = nextBit - START_LENGTH;
                uint8_t bytePtr = dataBit >> 3;
                uint8_t bitPtr = dataBit & 7;
                uint8_t bitValue = updateState.data[bytePtr] & (0b10000000 >> bitPtr); // MSB to LSB
                setOutput(bitValue);
                #ifdef ELVAS_DEBUG
                    dataBit % 8 || Serial.print(' ');
                    Serial.print(bitValue ? 'O' : '.');
                #endif
            } else {
                // tail
                setOutput(LOW);
            }

            if (nextBit < SEQUENCE_LENGTH - 1) {
                nextBit++;
            } else {
                // end of sequence
                nextBit = 0;
                digitalWrite(ledPin, false);
                #ifdef ELVAS_DEBUG
                    Serial.print(F(" ]"));
                #endif
            }
        } else {
            check();
        }
    }
}

void ElvasDisplay::setTime(Time time, bool tenths) {
    this->time = time;
    if (tenths) {
        setTimeSecTenth(time);
    } else {
        setTimeMinSec(time);
    }
    lastTimeChanged = millis();
}

void ElvasDisplay::setTimeMinSec(Time time) {
    currentState.fields.time3 = decimalDigit(time.fields.min, 1);
    currentState.fields.time2 = decimalDigit(time.fields.min, 0);
    currentState.fields.time1 = decimalDigit(time.fields.sec, 1);
    currentState.fields.time0 = decimalDigit(time.fields.sec, 0);
}

void ElvasDisplay::setTimeSecTenth(Time time) {
    currentState.fields.time3 = decimalDigit(time.fields.sec, 1);
    currentState.fields.time2 = decimalDigit(time.fields.sec, 0);
    currentState.fields.time1 = DIGIT_OFF; // off
    currentState.fields.time0 = decimalDigit(time.fields.tenth, 0);
}

void ElvasDisplay::setHomeScore(uint8_t score) {
    if (score <= MAX_SCORE) {
        currentState.fields.homeScore2 = decimalDigit(score, 2);
        currentState.fields.homeScore1 = decimalDigit(score, 1);
        currentState.fields.homeScore0 = decimalDigit(score, 0);
    }
}

void ElvasDisplay::setGuestScore(uint8_t score) {
    if (score <= MAX_SCORE) {
        currentState.fields.guestScore2 = decimalDigit(score, 2);
        currentState.fields.guestScore1 = decimalDigit(score, 1);
        currentState.fields.guestScore0 = decimalDigit(score, 0);
    }
}

void ElvasDisplay::setHomeFouls(uint8_t fouls) {
    if (fouls <= MAX_FOULS) {
        currentState.fields.homeFouls1 = decimalDigit(fouls, 1);
        currentState.fields.homeFouls0 = decimalDigit(fouls, 0);
    }
}

void ElvasDisplay::setGuestFouls(uint8_t fouls) {
    if (fouls <= MAX_FOULS) {
        currentState.fields.guestFouls1 = decimalDigit(fouls, 1);
        currentState.fields.guestFouls0 = decimalDigit(fouls, 0);
    }
}

void ElvasDisplay::setHomeTimeouts(uint8_t timeouts) {
    currentState.fields.homeTimeouts = timeouts == 1 ? 1 : timeouts > 1 ? 3 : 0;
    currentState.fields.homeService = timeouts == 3 ? 1 : 0; // use "service" indicator for 3rd timeout 
}

void ElvasDisplay::setGuestTimeouts(uint8_t timeouts) {
    currentState.fields.guestTimeouts = timeouts == 1 ? 1 : timeouts > 1 ? 3 : 0;
    currentState.fields.guestService = timeouts == 3 ? 1 : 0; // use "service" indicator for 3rd timeout 
}

void ElvasDisplay::setBuzzer(bool buzzer) {
    currentState.fields.buzzer = buzzer ? 1 : 0;
}

void ElvasDisplay::setUnknown1(bool value) {
    currentState.fields.unknown1 = value;
}

void ElvasDisplay::setUnknown2(bool value) {
    currentState.fields.unknown2 = value;
}

void ElvasDisplay::buzzerOn(unsigned long duration) {
    setUnknown2(false);
    setBuzzer(true);
    buzzerTimer.reset(duration);
}

void ElvasDisplay::buzzerOff() {
    setBuzzer(false);
    setUnknown2(true);
    buzzerTimer.stop();
}

void ElvasDisplay::endOfPeriod() {
    buzzerOn(END_OF_PERIOD_BUZZER_MS);
}

void ElvasDisplay::endOfTimeout() {
    buzzerOn(END_OF_TIMEOUT_BUZZER_MS);
}

void ElvasDisplay::loopBuzzer() {
    buzzerTimer.loop();
    if (buzzerTimer.isTriggered()) {
        buzzerOff();
    }
}

void ElvasDisplay::loopTimeDisplay() {
    if (state->isGamePeriod()) {
        if (time.fields.min == 0 && time.fields.sec > 0) {
            // flash time when paused during the last ten seconds
            showTime = (millis() - lastTimeChanged) / 250 % 2 == 0;
            showPeriod = false;
        } else {
            // show period once every 5 seconds
            showTime = true;
            showPeriod = (millis() - lastTimeChanged) / 1000 % 5 == 4;
        }
    }
}

void ElvasDisplay::loop() {
    loopBuzzer();
    loopTimeDisplay();
}

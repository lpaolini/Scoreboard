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
    reset();
}

void ElvasDisplay::reset() {
    for (uint8_t i = 0; i < DATA_LENGTH; i++) {
        currentState.data[i] = 0;
    }
    nextBit = 0;
    setUnknown1(true);
    setUnknown2(true);
    forceUpdate();
}

void ElvasDisplay::stateChange() {
    if (state->isGameMode()) {
        if (state->getPhase() == REGULAR_TIME || state->getPhase() == EXTRA_TIME) {
            showFouls = true;
            showTimeouts = true;
        } else {
            showFouls = false;
            showTimeouts = false;
        }
        timeDisplay = TIME;
        if (state->getMode() == STOP && state->getPhase() == REGULAR_TIME) {
            showPeriod = true;
            lastTimeStopped = millis();
        } else {
            showPeriod = false;
        }
    } else {
        showFouls = false;
        showTimeouts = false;
        timeDisplay = OFF;
        showPeriod = false;
    }
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
    digitalWrite(outputPin, level != INVERT_OUTPUT);
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
    switch (timeDisplay) {
        case OFF:
            nextState.fields.time3 = DIGIT_OFF;
            nextState.fields.time2 = DIGIT_OFF;
            nextState.fields.time1 = DIGIT_OFF;
            nextState.fields.time0 = DIGIT_OFF;
            break;
        case PERIOD:
            nextState.fields.time3 = DIGIT_OFF;
            nextState.fields.time2 = DIGIT_OFF;
            nextState.fields.time1 = state->getPeriod();
            nextState.fields.time0 = DIGIT_OFF;
            break;
        default:
            break;
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

void ElvasDisplay::forceUpdate() {
    updateRequired = true;
}

void ElvasDisplay::update() {
    if (updateRequired || nextBit != 0) {
        if (nextBit == 0) {
            // create data snapshop and re-arm trigger
            copyState(updateState.data, nextState.data);
            updateRequired = false;
            digitalWrite(ledPin, true);
            #ifdef SERIAL_DEBUG
                Serial.print("Elvas:");
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
            #ifdef SERIAL_DEBUG
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
            #ifdef SERIAL_DEBUG
                Serial.println();
            #endif
        }
    } else {
        check();
    }
}

void ElvasDisplay::setTime(unsigned long time) {
    if (time <= 59900 && state->isGamePeriod()) {
        setTimeSecTenth(time);
    } else {
        setTimeMinSec(time);
    }
}

void ElvasDisplay::setTimeMinSec(unsigned long time) {
    unsigned long adjustedTime = time + 999; // adjust for truncation to whole seconds
    uint8_t min = adjustedTime / 60000;
    uint8_t sec = adjustedTime % 60000 / 1000;
    currentState.fields.time3 = decimalDigit(min, 1);
    currentState.fields.time2 = decimalDigit(min, 0);
    currentState.fields.time1 = decimalDigit(sec, 1);
    currentState.fields.time0 = decimalDigit(sec, 0);
}

void ElvasDisplay::setTimeSecTenth(unsigned long time) {
    unsigned long adjustedTime = time + 99; // adjust for truncation to tenths of seconds
    uint8_t sec = adjustedTime % 60000 / 1000;
    uint8_t tenth = adjustedTime % 1000 / 100;
    currentState.fields.time3 = decimalDigit(sec, 1);
    currentState.fields.time2 = decimalDigit(sec, 0);
    currentState.fields.time1 = DIGIT_OFF; // off
    currentState.fields.time0 = decimalDigit(tenth, 0);

    if (sec == 0 && tenth == 0) {
        setUnknown2(false);
        setBuzzer(true);
        buzzer.reset();
    }
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

void ElvasDisplay::loopBuzzer() {
    buzzer.loop();
    if (buzzer.isTriggered()) {
        setBuzzer(false);
        setUnknown2(true);
    }
}

void ElvasDisplay::loopTimeDisplay() {
    if (showPeriod) {
        timeDisplay = (millis() - lastTimeStopped) / 1000 % 4 >= 2 ? PERIOD : TIME;
    }
}

void ElvasDisplay::loop() {
    loopBuzzer();
    loopTimeDisplay();
}

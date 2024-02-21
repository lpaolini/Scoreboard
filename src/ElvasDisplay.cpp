#include "ElvasDisplay.h"

ElvasDisplay::ElvasDisplay(uint8_t outputPin, uint8_t ledPin) {
    this->outputPin = outputPin;
    this->ledPin = ledPin;
    init();
}

void ElvasDisplay::setup() {
    pinMode(outputPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    reset();
}

void ElvasDisplay::reset() {
    for (uint8_t i = 0; i < DATA_LENGTH; i++) {
        state.data[i] = 0;
    }
    nextBit = 0;
    setUnknown1(true);
    setUnknown2(true);
    forceUpdate();
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
    copyState(nextData, state.data);
    updateRequired = memcmp(nextData, data, DATA_LENGTH * sizeof(uint8_t)) != 0;
}

void ElvasDisplay::forceUpdate() {
    updateRequired = true;
}

void ElvasDisplay::update() {
    if (updateRequired || nextBit != 0) {
        if (nextBit == 0) {
            // create data snapshop and re-arm trigger
            copyState(data, nextData);
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
            uint8_t bitValue = data[bytePtr] & (0b10000000 >> bitPtr); // MSB to LSB
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
    if (time <= 59900) {
        setTimeSecTenth(time);
    } else {
        setTimeMinSec(time);
    }
}

void ElvasDisplay::setTimeMinSec(unsigned long time) {
    unsigned long adjustedTime = time + 999; // adjust for truncation to whole seconds
    uint8_t min = adjustedTime / 60000;
    uint8_t sec = adjustedTime % 60000 / 1000;
    state.fields.time3 = decimalDigit(min, 1);
    state.fields.time2 = decimalDigit(min, 0);
    state.fields.time1 = decimalDigit(sec, 1);
    state.fields.time0 = decimalDigit(sec, 0);
}

void ElvasDisplay::setTimeSecTenth(unsigned long time) {
    unsigned long adjustedTime = time + 99; // adjust for truncation to tenths of seconds
    uint8_t sec = adjustedTime % 60000 / 1000;
    uint8_t tenth = adjustedTime % 1000 / 100;
    state.fields.time3 = decimalDigit(sec, 1);
    state.fields.time2 = decimalDigit(sec, 0);
    state.fields.time1 = DIGIT_OFF; // off
    state.fields.time0 = decimalDigit(tenth, 0);

    if (sec == 0 && tenth == 0) {
        setUnknown2(false);
        setBuzzer(true);
        buzzer.reset();
    }
}

void ElvasDisplay::setHomeScore(uint8_t score) {
    if (score <= MAX_SCORE) {
        state.fields.homeScore2 = decimalDigit(score, 2);
        state.fields.homeScore1 = decimalDigit(score, 1);
        state.fields.homeScore0 = decimalDigit(score, 0);
    }
}

void ElvasDisplay::setGuestScore(uint8_t score) {
    if (score <= MAX_SCORE) {
        state.fields.guestScore2 = decimalDigit(score, 2);
        state.fields.guestScore1 = decimalDigit(score, 1);
        state.fields.guestScore0 = decimalDigit(score, 0);
    }
}

void ElvasDisplay::setHomeFouls(uint8_t fouls) {
    if (fouls <= MAX_FOULS) {
        state.fields.homeFouls1 = decimalDigit(fouls, 1);
        state.fields.homeFouls0 = decimalDigit(fouls, 0);
    }
}

void ElvasDisplay::setGuestFouls(uint8_t fouls) {
    if (fouls <= MAX_FOULS) {
        state.fields.guestFouls1 = decimalDigit(fouls, 1);
        state.fields.guestFouls0 = decimalDigit(fouls, 0);
    }
}

void ElvasDisplay::setHomeTimeouts(uint8_t timeouts) {
    state.fields.homeTimeouts = timeouts == 1 ? 1 : timeouts > 1 ? 3 : 0;
    state.fields.homeService = timeouts == 3 ? 1 : 0;
}

void ElvasDisplay::setGuestTimeouts(uint8_t timeouts) {
    state.fields.guestTimeouts = timeouts == 1 ? 1 : timeouts > 1 ? 3 : 0;
    state.fields.guestService = timeouts == 3 ? 1 : 0;
}

void ElvasDisplay::setBuzzer(bool buzzer) {
    state.fields.buzzer = buzzer ? 1 : 0;
}

void ElvasDisplay::setUnknown1(bool value) {
    state.fields.unknown1 = value;
}

void ElvasDisplay::setUnknown2(bool value) {
    state.fields.unknown2 = value;
}

void ElvasDisplay::loopBuzzer() {
    buzzer.loop();
    if (buzzer.isTriggered()) {
        setBuzzer(false);
        setUnknown2(true);
    }
}

void ElvasDisplay::loop() {
    loopBuzzer();
}

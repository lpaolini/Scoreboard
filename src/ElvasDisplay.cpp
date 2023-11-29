#include "ElvasDisplay.h"

ElvasDisplay::ElvasDisplay(uint8_t outputPin) {
    this->outputPin = outputPin;
    init();
}

void ElvasDisplay::init() {
    for (uint8_t i = 0; i < DATA_LENGTH; i++) {
        state.data[i] = 0;
    }
    nextBit = 0;

    pinMode(outputPin, OUTPUT);

    // Timer1.initialize(BIT_DURATION_MICROSECONDS);
    // Timer1.attachInterrupt([this] { this->update(); });
    // Timer1.start();

    // timer.begin([this] { this->update(); }, BIT_DURATION_MICROSECONDS);

    // timer.every(BIT_DURATION_MICROSECONDS, onTimer, this);
}

// void ElvasDisplay::onTimer(void *ptr) {
//     ((ElvasDisplay *)ptr)->update();
// }

void ElvasDisplay::setup() {
    // timer.setPeriod(BIT_DURATION_MICROSECONDS);
    // timer.attachInterrupt(&ElvasDisplay::update, this);
    // timer.start();
    // timer.begin([this] { this->update(); }, BIT_DURATION_MICROSECONDS);
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

void ElvasDisplay::triggerUpdate() {
    triggeredUpdate = true;
}

void ElvasDisplay::update() {
    static uint8_t data[DATA_LENGTH];
    if (continuousUpdate || triggeredUpdate || nextBit != 0) {

        if ((continuousUpdate || triggeredUpdate) && nextBit == 0) {
            // create data snapshop and re-arm trigger
            noInterrupts();
            memcpy(data, state.data, DATA_LENGTH * sizeof(uint8_t));
            interrupts();
            triggeredUpdate = false;
        }

        if (nextBit < START_LENGTH) {
            // head (start pulse)
            setOutput(HIGH);
        } else if (nextBit > 110) {
            // tail (no data)
            setOutput(LOW);
        } else {
            // data
            uint8_t dataBit = nextBit - START_LENGTH;
            uint8_t bytePtr = dataBit >> 3;
            uint8_t bitPtr = dataBit & 7;
            uint8_t bitValue = data[bytePtr] & (0b10000000 >> bitPtr); // MSB to LSB
            setOutput(bitValue);
        }

        if (nextBit == SEQUENCE_LENGTH - 1) {
            nextBit = 0;
        } else {
            nextBit++;
        }
    }
}

void ElvasDisplay::setTime(unsigned long time) {
    if (time > 0) {
        uint8_t min = time / 60000;
        uint8_t sec = time % 60000 / 1000;
        uint8_t tenth = time % 1000 / 100;
        if (min == 0) {
            state.fields.time3 = decimalDigit(sec, 1);
            state.fields.time2 = decimalDigit(sec, 0);
            state.fields.time1 = decimalDigit(tenth, 0);
            state.fields.time0 = 0;
        } else {
            state.fields.time3 = decimalDigit(min, 1);
            state.fields.time2 = decimalDigit(min, 0);
            state.fields.time1 = decimalDigit(sec, 1);
            state.fields.time0 = decimalDigit(sec, 0);
        }
        triggerUpdate();
    } else {
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
    triggerUpdate();
}

void ElvasDisplay::setGuestScore(uint8_t score) {
    if (score <= MAX_SCORE) {
        state.fields.guestScore2 = decimalDigit(score, 2);
        state.fields.guestScore1 = decimalDigit(score, 1);
        state.fields.guestScore0 = decimalDigit(score, 0);
    }
    triggerUpdate();
}

void ElvasDisplay::setHomeFouls(uint8_t fouls) {
    if (fouls <= MAX_FOULS) {
        state.fields.homeFouls1 = decimalDigit(fouls, 1);
        state.fields.homeFouls0 = decimalDigit(fouls, 0);
    }
    triggerUpdate();
}

void ElvasDisplay::setGuestFouls(uint8_t fouls) {
    if (fouls <= MAX_FOULS) {
        state.fields.guestFouls1 = decimalDigit(fouls, 1);
        state.fields.guestFouls0 = decimalDigit(fouls, 0);
    }
    triggerUpdate();
}

void ElvasDisplay::setHomeTimeouts(uint8_t timeouts) {
    state.fields.homeTimeouts = timeouts == 1 ? 1 : timeouts > 1 ? 3 : 0;
    state.fields.homeService = timeouts == 3 ? 1 : 0;
    triggerUpdate();
}

void ElvasDisplay::setGuestTimeouts(uint8_t timeouts) {
    state.fields.guestTimeouts = timeouts == 1 ? 1 : timeouts == 2 ? 3 : 0;
    state.fields.guestService = timeouts == 3 ? 1 : 0;
    triggerUpdate();
}

void ElvasDisplay::setHomeService(uint8_t service) {
    state.fields.homeService = service;
    triggerUpdate();
}

void ElvasDisplay::setGuestService(uint8_t service) {
    state.fields.guestService = service;
    triggerUpdate();
}

void ElvasDisplay::setBuzzer(bool buzzer) {
    state.fields.buzzer = buzzer ? 1 : 0;
    triggerUpdate();
}

void ElvasDisplay::setUnknown1(bool value) {
    state.fields.unknown1 = value;
    triggerUpdate();
}

void ElvasDisplay::setUnknown2(bool value) {
    state.fields.unknown2 = value;
    triggerUpdate();
}

void ElvasDisplay::setContinuousUpdate(bool continuousUpdate) {
    this->continuousUpdate = continuousUpdate;
}

void ElvasDisplay::loopBuzzer() {
    buzzer.loop();
    if (buzzer.isExpired()) {
        setBuzzer(false);
    }
}

void ElvasDisplay::loop() {
    // timer.tick();
    loopBuzzer();
}

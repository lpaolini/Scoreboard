#include "Beeper.h"

Beeper::Beeper(
    uint8_t pin
) {
    this->pin = pin;
}

void Beeper::setup() {
    pinMode(pin, OUTPUT);
}

void Beeper::ready() {
    play(READY);
}

void Beeper::click() {
    play(CLICK);
}

void Beeper::timeStart() {
    play(TIME_START);
}

void Beeper::timeStop() {
    play(TIME_STOP);
}

void Beeper::lastTwoMinutes() {
    play(LAST_TWO_MINUTES);
}

void Beeper::confirm() {
    play(CONFIRMATION);
}

void Beeper::notAllowed() {
    play(NOT_ALLOWED);
}

void Beeper::play(Tone *sequence) {
    this->sequence = sequence;
    step = 0;
}

void Beeper::loop() {
    if (sequence != nullptr) {
        if (step == 0 || (millis() - time > sequence[step - 1].duration)) {
            Tone t = sequence[step];
            if (t.frequency == 0 && t.duration == 0) {
                sequence = nullptr;
            } else {
                tone(pin, t.frequency, t.duration);
                time = millis();
                step++;
            }
        }
    }
}

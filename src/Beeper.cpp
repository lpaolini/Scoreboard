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

void Beeper::confirm(void (*callback)()) {
    this->callback = callback;
    play(CONFIRM);
}

void Beeper::notAllowed() {
    play(NOT_ALLOWED);
}

void Beeper::alert(bool enable) {
    if (enable) {
        play(ALERT, true);
    } else {
        stop();
    }
}

void Beeper::play(Tone *sequence, bool looped) {
    this->sequence = sequence;
    this->looped = looped;
    step = 0;
}

void Beeper::stop() {
    sequence = nullptr;
}

void Beeper::loop() {
    if (sequence != nullptr) {
        if (step == 0 || (millis() - time > sequence[step - 1].duration)) {
            Tone t = sequence[step];
            if (t.frequency == 0 && t.duration == 0) {
                if (looped) {
                    step = 0;
                } else {
                    sequence = nullptr;
                    if (callback != nullptr) {
                        callback();
                        callback = nullptr;
                    }
                }
            } else {
                if (t.frequency) {
                    #ifdef SOUND
                        tone(pin, t.frequency, t.duration);
                    #endif
                }
                time = millis();
                step++;
            }
        }
    }
}

#ifndef State_h
#define State_h

#include <Arduino.h>

enum Mode {SET_STEP, SET_TIME, GAME};
enum Phase {PREPARATION, REGULAR_TIME, INTERVAL, EXTRA_TIME, END_OF_GAME};

class State {
    private:
        void (*onUpdate)(Mode mode, Phase phase, uint8_t period) = nullptr;

        Mode mode = SET_STEP;
        Phase phase = PREPARATION;
        uint8_t period = 1;

        void update() {
            if (onUpdate != nullptr) {
                onUpdate(mode, phase, period);
            }
        }

    public:
        State() {};

        void setOnUpdate(void (*onUpdate)(Mode mode, Phase phase, uint8_t period)) {
            this->onUpdate = onUpdate;
        }

        void set(Mode mode, Phase phase, uint8_t period) {
            this->mode = mode;
            this->phase = phase;
            this->period = period;
            update();
        }

        void setMode(Mode mode) {
            this->mode = mode;
            update();
        }

        Mode getMode() {
            return mode;
        }

        void setPhase(Phase phase) {
            this->phase = phase;
            update();
        }

        Phase getPhase() {
            return phase;
        }

        void setPeriod(uint8_t period) {
            this->period = period;
            update();
        }

        uint8_t getPeriod() {
            return period;
        }
};

#endif

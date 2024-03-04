#ifndef State_h
#define State_h

#include <Arduino.h>

enum Mode {STOP, RUN, SET_STEP, SET_TIME};
enum Phase {PREPARATION, REGULAR_TIME, INTERVAL, EXTRA_TIME, END_OF_GAME};

// enum Time {STOP, RUN}
// enum Mode {SET_STEP, SET_TIME, GAME};
// enum Phase {PREPARATION, REGULAR_TIME, INTERVAL, EXTRA_TIME, END_OF_GAME};

class State {
    private:
        void (*onUpdate)() = nullptr;

        Mode mode = SET_STEP;
        Phase phase = REGULAR_TIME;
        uint8_t period = 1;

        void update() {
            if (onUpdate != nullptr) {
                onUpdate();
            }
        }

    public:
        State() {};

        void setOnUpdate(void (*onUpdate)()) {
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

        bool isStartOfGame() {
            return phase == PREPARATION || (phase == REGULAR_TIME && period == 1);
        }

        bool isGamePeriod() {
            return isGameMode() && (phase == REGULAR_TIME || phase == EXTRA_TIME);
        }

        bool isGameMode() {
            return mode == RUN || mode == STOP;
        }

        uint8_t getMaxTimeouts() {
            if (phase == REGULAR_TIME) {
                if (period <= 2) {
                    return 2;
                } else {
                    return 3;
                }
            }
            if (phase == EXTRA_TIME) {
                return 1;
            }
            return 0;
        }
};

#endif

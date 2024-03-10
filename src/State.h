#ifndef State_h
#define State_h

#include <Arduino.h>

enum Mode {RESET, SET_STEP, SET_TIME, GAME};
enum Phase {PREPARATION, REGULAR_TIME, INTERVAL, EXTRA_TIME};
enum Chrono {STOP, RUN};

class State {
    private:
        void (*onUpdate)() = nullptr;

        Mode mode = RESET;
        Phase phase = REGULAR_TIME;
        Chrono chrono = STOP;
        uint8_t period = 1;

        void update() {
            #ifdef STATE_DEBUG
                Serial.print(F("\nState: mode = "));
                Serial.print(mode);
                Serial.print(F(", phase = "));
                Serial.print(phase);
                Serial.print(F(", chrono = "));
                Serial.print(chrono);
                Serial.print(F(", period = "));
                Serial.print(period);
            #endif
            if (onUpdate != nullptr) {
                onUpdate();
            }
        }

    public:
        State() {};

        void setup(void (*onUpdate)()) {
            this->onUpdate = onUpdate;
        }

        void reset() {
            mode = SET_STEP;
            phase = REGULAR_TIME;
            chrono = STOP;
            period = 1;
            update();
        }
 
        void setMode(Mode mode) {
            if (this->mode != mode) {
                this->mode = mode;
                update();
            }
        }

        Mode getMode() {
            return mode;
        }

        void setPhase(Phase phase) {
            if (this->phase != phase) {
                this->phase = phase;
                update();
            }
        }

        Phase getPhase() {
            return phase;
        }

        void setChrono(Chrono chrono) {
            if (this->chrono != chrono) {
                this->chrono = chrono;
                update();
            }
        }

        Chrono getChrono() {
            return chrono;
        }

        void setPeriod(uint8_t period) {
            if (this->period != period) {
                this->period = period;
                update();
            }
        }

        uint8_t getPeriod() {
            return phase == REGULAR_TIME ? period : 0;
        }

        bool isGameMode() {
            return mode == GAME;
        }

        bool isStartOfGame() {
            return mode == GAME && (phase == PREPARATION || (phase == REGULAR_TIME && period == 1));
        }

        bool isGamePeriod() {
            return mode == GAME && (phase == REGULAR_TIME || phase == EXTRA_TIME);
        }

        bool isFourthPeriod() {
            return mode == GAME && (phase == REGULAR_TIME && period == 4);
        }

        bool isFourthPeriodOrOvertime() {
            return mode == GAME && ((phase == REGULAR_TIME && period == 4) || phase == EXTRA_TIME);
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

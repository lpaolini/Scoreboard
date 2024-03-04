#ifndef ElvasDisplay_h
#define ElvasDisplay_h

#include <Arduino.h>
#include "Timer.h"
#include "Time.h"
#include "State.h"
#include "WallDisplay.h"
#include "constants.h"

#define BIT_DURATION_MICROSECONDS 1149

class ElvasDisplay : public WallDisplay {
    private:
        static const uint8_t DATA_LENGTH = 13;
        const uint8_t START_LENGTH = 9;
        const uint8_t SEQUENCE_LENGTH = 120;
        const bool INVERT_OUTPUT = false;
        const uint16_t BUZZER_DURATION_MS = 3000;
        const uint8_t DIGIT_OFF = 10;

        typedef union {
            struct Fields {
                // byte 0
                uint8_t time0          : 4; // 000
                uint8_t                : 4; // 004
                // byte 1
                uint8_t time1          : 4; // 008
                uint8_t                : 4; // 012
                // byte 2
                uint8_t time2          : 4; // 016
                uint8_t                : 4; // 020
                // byte 3
                uint8_t time3          : 4; // 024
                uint8_t                : 4; // 028
                // byte 4
                uint8_t guestFouls0    : 4; // 032
                uint8_t                : 4; // 036
                // byte 5
                uint8_t homeFouls0     : 4; // 040
                uint8_t                : 4; // 044
                // byte 6
                uint8_t guestScore0    : 4; // 048
                uint8_t                : 4; // 052
                // byte 7
                uint8_t guestScore1    : 4; // 056
                uint8_t                : 4; // 060
                // byte 8
                uint8_t homeScore0     : 4; // 064
                uint8_t                : 4; // 068
                // byte 9
                uint8_t homeScore1     : 4; // 072
                uint8_t                : 4; // 076
                // byte 10
                uint8_t unknown1       : 1; // 087
                uint8_t                : 7; // 080
                // byte 11
                uint8_t unknown2       : 1; // 095
                uint8_t                : 3; // 092
                uint8_t homeScore2     : 1; // 091
                uint8_t guestScore2    : 1; // 090
                uint8_t homeFouls1     : 1; // 089
                uint8_t guestFouls1    : 1; // 088
                // byte 12
                uint8_t homeService    : 1; // 103 
                uint8_t guestService   : 1; // 102
                uint8_t homeTimeouts   : 2; // 100
                uint8_t guestTimeouts  : 2; // 098
                uint8_t buzzer         : 1; // 097
                uint8_t                : 1; // 096
            } fields;
            uint8_t data[DATA_LENGTH];
        } ElvasDisplayState;

        State *state;

        ElvasDisplayState currentState;
        ElvasDisplayState nextState;
        ElvasDisplayState updateState;

        uint8_t outputPin;
        uint8_t ledPin;
        Timer buzzerTimer = Timer(BUZZER_DURATION_MS, false);

        bool showFouls;
        bool showTimeouts;
        bool showPeriod;
        unsigned long lastTimeChanged;

        volatile uint8_t nextBit;
        int decimalDigit(int value, int digit);
        bool updateRequired = false;
        void copyState(void *dst, const void *src);
        void check();
        void alterTimeDisplay();
        void alterFoulsDisplay();
        void alterTimeoutDisplay();
        void setOutput(bool level);
        void setTimeMinSec(Time time);
        void setTimeSecTenth(Time time);
        void setUnknown1(bool value);
        void setUnknown2(bool value);
        void buzzer(bool enabled);
        void loopBuzzer();
        void loopTimeDisplay();

    public:
        ElvasDisplay(uint8_t outputPin, uint8_t ledPin, State *state);
        void setup();
        void reset();
        void update();
        void stateChange();
        void setTime(Time time, bool tenths);
        void setHomeScore(uint8_t score);
        void setGuestScore(uint8_t score);
        void setHomeFouls(uint8_t fouls);
        void setGuestFouls(uint8_t fouls);
        void setHomeTimeouts(uint8_t timeouts);
        void setGuestTimeouts(uint8_t timeouts);
        void setBuzzer(bool buzzer);
        void loop();
};

#endif

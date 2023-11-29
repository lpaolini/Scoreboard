#ifndef Extra_h
#define Extra_h

#include <Arduino.h>
#include <LedControl.h>
#include "Timer.h"
#include "constants.h"

// Fouls

// normal       inverted
// --------     --------
// F 0  xxx     xxx  F 0
// F10  xxx     xxx  F10

// Timeouts

// normal       inverted
// --------     --------
// xxx    o     o    xxx
// xxx   oo     oo   xxx
// xxx  ooo     ooo  xxx

class Extra {
    private:
        const unsigned long INPUT_TIMER_MS = 1000;
        const unsigned long CONFIRMATION_FLASH_COUNT = 2;
        const unsigned long CONFIRMATION_FLASH_DURATION_MS = 100;
        const uint8_t FOULS_POS[2][3] = {{ 5, 6, 7 }, { 0, 1, 2 }};
        const uint8_t TIMEOUTS_POS[2][3] = {{ 0, 1, 2 }, { 7, 6, 5 }};

        LedControl *display;
        uint8_t displayIndex;
        uint8_t brightness;
        bool invert;
        void (*onUpdateFouls)(uint8_t fouls);
        void (*onUpdateTimeouts)(uint8_t timeouts);

        uint8_t fouls = 0;
        uint8_t timeouts = 0;
        bool updating = false;
        uint8_t prevFouls = 0;
        uint8_t prevTimeouts = 0;
        Timer inputTimer = Timer(INPUT_TIMER_MS, false);
        Timer foulsFlashTimer = Timer(2 * CONFIRMATION_FLASH_COUNT * CONFIRMATION_FLASH_DURATION_MS, false);
        Timer timeoutsFlashTimer = Timer(2 * CONFIRMATION_FLASH_COUNT * CONFIRMATION_FLASH_DURATION_MS, false);
        int decimalDigit(int value, int digit);
        void printTimeoutChar(uint8_t pos, bool show);
        void resetFouls();
        void resetTimeouts();
        void update();
        void updateFouls(bool show = true);
        void updateTimeouts(bool show = true);
        void loopInput();
        void loopFoulsFlash();
        void loopTimeoutsFlash();

    public:
        Extra(
            LedControl *display, 
            uint8_t displayIndex,
            uint8_t brightness,
            bool invert,
            void (*onUpdateFouls)(uint8_t fouls),
            void (*onUpdateTimeouts)(uint8_t timeouts)
        );
        void setup();
        void reset();
        void resetPeriod();
        void increaseFouls();
        void decreaseFouls();
        void increaseTimeouts();
        void decreaseTimeouts();
        void loop();
};

#endif

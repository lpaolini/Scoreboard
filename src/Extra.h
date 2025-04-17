#ifndef Extra_h
#define Extra_h

#include <Arduino.h>
#include <LedControl.h>
#include "constants.h"
#include "Beeper.h"
#include "Timer.h"
#include "State.h"

// Fouls

// normal       inverted
// --------     --------
// F 0  xxx     xxx  F 0
// F10  xxx     xxx  F10
// bon  xxx     xxx  bon

// Timeouts

// normal       inverted
// --------     --------
// xxx    o     o    xxx
// xxx   oo     oo   xxx
// xxx  ooo     ooo  xxx

class Extra {
    private:
        enum FoulsDisplayMode {SHOW_FOULS, SHOW_BONUS, FOULS_OFF};
        enum TimeoutsDisplayMode {SHOW_TIMEOUTS, TIMEOUTS_OFF};

        const unsigned long INPUT_TIMER_MS = 600;
        const unsigned long CONFIRMATION_FLASH_COUNT = 6;
        const unsigned long CONFIRMATION_FLASH_DURATION_MS = 75;
        const unsigned long BONUS_FLASH_COUNT = 30;
        const unsigned long BONUS_FLASH_DURATION_MS = 250;
        const uint8_t FOULS_POS[2][3] = {{ 5, 6, 7 }, { 0, 1, 2 }};
        const uint8_t TIMEOUTS_POS[2][3] = {{ 0, 1, 2 }, { 7, 6, 5 }};

        LedControl *display;
        uint8_t displayIndex;
        uint8_t brightness;
        bool invert;
        State *state;
        Beeper *beeper;

        void (*onUpdateFouls)(uint8_t fouls) {};
        void (*onUpdateTimeouts)(uint8_t timeouts, bool increased) {};

        bool enabled;

        uint8_t fouls = 0;
        uint8_t timeouts = 0;
        uint8_t maxTimeouts = 0;
        bool updating = false;
        uint8_t prevFouls = 0;
        uint8_t prevTimeouts = 0;
        Timer inputTimer = Timer(INPUT_TIMER_MS, false);
        Timer foulsConfirmationTimer = Timer(2 * CONFIRMATION_FLASH_COUNT * CONFIRMATION_FLASH_DURATION_MS, false);
        Timer timeoutsConfirmationTimer = Timer(2 * CONFIRMATION_FLASH_COUNT * CONFIRMATION_FLASH_DURATION_MS, false);
        Timer bonusConfirmationTimer = Timer(2 * BONUS_FLASH_COUNT * BONUS_FLASH_DURATION_MS, false);
        void enable(bool enabled);
        int decimalDigit(int value, int digit);
        void printTimeoutChar(uint8_t pos, bool show);
        void resetFouls();
        void updateFouls(uint8_t fouls);
        void resetTimeouts();
        void updateTimeouts(uint8_t timeouts);
        void updateFoulsDisplay(FoulsDisplayMode foulsDisplayMode);
        void publishFouls();
        void updateTimeoutsDisplay(TimeoutsDisplayMode timeoutsDisplayMode);
        void publishTimeouts(bool increased = false);
        void loopInput();
        void loopFoulsConfirmation();
        void loopBonusConfirmation();
        void loopTimeoutsConfirmation();

    public:
        Extra(
            LedControl *display, 
            uint8_t displayIndex,
            uint8_t brightness,
            bool invert,
            State *state,
            Beeper *beeper
        );
        void setup(
            void (*onUpdateFouls)(uint8_t fouls), 
            void (*onUpdateTimeouts)(uint8_t timeouts, bool increased)
        );
        void reset();
        bool isEnabled();
        void stateChange();
        void resetPeriod();
        void increaseFouls();
        void decreaseFouls();
        void increaseTimeouts();
        void decreaseTimeouts();
        void lastTwoMinutes();
        void loop();
};

#endif

#ifndef SingleScore_h
#define SingleScore_h

#include <Arduino.h>
#include <LedControl.h>
#include "constants.h"
#include "Beeper.h"
#include "Timer.h"
#include "State.h"

// Score

// normal       inverted
// --------     --------
// xxx  yyy     yyy  xxx
// 00   yyy     yyy   00
// 99   yyy     yyy   99
// 199  yyy     yyy  199
// xxx   0P      0P  xxx
// xxx   1P      1P  xxx
// xxx  -1P     -1P  xxx


class Score {
    private:
        enum DeltaMode {NORMAL, UPDATE, ALTER};
        // make sure INPUT_TIMER_MS is not less than 2 * FLASH_TIMER_MS * FLASH_COUNT
        const unsigned long INPUT_TIMER_MS = 600;
        const unsigned long CONFIRMATION_FLASH_COUNT = 6;
        const unsigned long CONFIRMATION_FLASH_DURATION_MS = 75;
        const bool THREE_DIGIT_SCORE = false;
        const uint8_t SCORE_POS[2][3] = {{ 5, 6, 7 }, { 0, 1, 2 }};
        const uint8_t DELTA_POS[2][3] = {{ 0, 1, 2 }, { 5, 6, 7 }};

        LedControl *display;
        uint8_t displayIndex;
        uint8_t brightness;
        bool invert;
        State *state;
        Beeper *beeper;

        void (*onUpdate)(uint8_t score) {};

        bool enabled;
        DeltaMode deltaMode;

        uint8_t score;
        uint8_t prevScore;
        int8_t delta;
        int8_t prevDelta;
        bool updating;
        bool altering;
        Timer inputTimer = Timer(INPUT_TIMER_MS, false);
        Timer flashTimer = Timer(2 * CONFIRMATION_FLASH_COUNT * CONFIRMATION_FLASH_DURATION_MS, false);
        void enable(bool enabled);
        void resetScore();
        void resetDelta();
        int decimalDigit(int value, int digit);
        uint8_t limitScore(int16_t score);
        void updateScoreDisplay(bool show = true);
        void updateDeltaDisplay(int8_t delta, DeltaMode deltaMode);
        void publishScore();
        void loopInput();
        void loopFlash();

    public:
        Score(
            LedControl *display, 
            uint8_t displayIndex,
            uint8_t brightness,
            bool invert,
            State *state,
            Beeper *beeper
        );
        void setup(void (*onUpdate)(uint8_t score));
        void reset();
        bool isEnabled();
        void stateChange();
        void resetPeriod();
        void startTimer();
        void increaseDelta(bool roll = true);
        void decreaseDelta(bool roll = true);
        void clearDelta();
        void increaseScore();
        void decreaseScore();
        void alterScore(bool enabled);
        void undo();
        void loop();
};

#endif

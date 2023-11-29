#ifndef SingleScore_h
#define SingleScore_h

#include <Arduino.h>
#include <LedControl.h>
#include "Timer.h"
#include "constants.h"

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
        // make sure INPUT_TIMER_MS is not less than 2 * FLASH_TIMER_MS * FLASH_COUNT
        const unsigned long INPUT_TIMER_MS = 1000;
        const unsigned long CONFIRMATION_FLASH_COUNT = 2;
        const unsigned long COMFIRMATION_FLASH_DURATION_MS = 100;
        const uint8_t SCORE_POS[2][3] = {{ 5, 6, 7 }, { 0, 1, 2 }};
        const uint8_t DELTA_POS[2][3] = {{ 0, 1, 2 }, { 5, 6, 7 }};

        LedControl *display;
        uint8_t displayIndex;
        uint8_t brightness;
        bool invert;
        void (*onUpdate)(uint8_t score);

        uint8_t score;
        int8_t delta;
        int8_t prevDelta;
        bool updating;
        Timer inputTimer = Timer(INPUT_TIMER_MS, false);
        Timer flashTimer = Timer(2 * CONFIRMATION_FLASH_COUNT * COMFIRMATION_FLASH_DURATION_MS, false);
        int decimalDigit(int value, int digit);
        void increaseDelta(bool roll = false);
        // void decreaseDelta(bool roll = false);
        uint8_t limitScore(int16_t score);
        void updateScore(bool show = true);
        void updateDelta(int8_t delta, bool showIndicator = true);
        void loopInput();
        void loopFlash();

    public:
        Score(
            LedControl *display, 
            uint8_t displayIndex,
            uint8_t brightness,
            bool invert,
            void (*onUpdate)(uint8_t score)
        );
        void setup();
        void reset();
        void startTimer();
        void nextDelta();
        void clearDelta();
        void increaseScore();
        void decreaseScore();
        void undo();
        void loop();
};

#endif

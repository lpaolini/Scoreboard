#ifndef GameTime_h
#define GameTime_h

#include <Arduino.h>
#include <Adafruit_LEDBackpack.h>
#include "Beeper.h"
#include "Timer.h"

class GameTime {
    private:
        const unsigned long MIN = 60000; // one minute
        const unsigned long MAX_TIME = 20 * MIN; // 20 minutes
        const unsigned long preset[7] {3 * MIN, 5 * MIN, 8 * MIN, 10 * MIN, 12 * MIN, 15 * MIN, 20 * MIN};
        const uint8_t defaultPreset = 3;
        const uint8_t START_FLASH_BRIGHTNESS = 15;
        const uint16_t START_FLASH_DURATION_MS = 100;
        const uint16_t STOP_FLASH_DURATION_MS = 150;

        Adafruit_7segment *display;
        uint8_t address;
        uint8_t brightness;
        Beeper *beeper;

        void (*onUpdate)(unsigned long time) {};
        void (*onResetPeriod)(uint8_t period) {};
        void (*onGameMode)(bool gameMode) {};
        void (*onLastTwoMinutes)() {};

        uint8_t currentPreset = defaultPreset;

        enum Mode {STOP, RUN, SET_STEP, SET_TIME};
        enum Phase {PREPARATION, REGULAR_TIME, INTERVAL, EXTRA_TIME, END_OF_GAME};

        Mode mode;
        Phase phase;
        uint8_t period;
        uint8_t homeScore;
        uint8_t guestScore;

        unsigned long lastTime;
        unsigned long time;
        unsigned long timeStart;
        unsigned long timeStop;

        Timer hold = Timer(1000, false);
        Timer startFlash = Timer(START_FLASH_DURATION_MS, false);

        void setBrightness(uint8_t brightness);
        int decimalDigit(int value, int digit);
        uint8_t presetCount();
        void showTime();
        void showMinSec(unsigned long time);
        void showSecTenth(unsigned long time);
        void showLastTwoMinutesAlert(unsigned long time);
        void setTime();
        void showPeriod();
        void increaseRemainingTime();
        void increaseStep();
        void increasePeriod();
        void increaseTime();
        void decreaseRemainingTime();
        void decreaseStep();
        void decreasePeriod();
        void decreaseTime();
        bool isParity();
        void loopRun();
        void loopStop();
        void loopSetTime();
        void loopSetStep();

    public:
        GameTime(
            Adafruit_7segment *display,
            uint8_t address,
            uint8_t brightness,
            Beeper *beeper
        );
        void setup(
            void (*onUpdate)(unsigned long remaining),
            void (*onGameMode)(bool gameMode),
            void (*onResetPeriod)(uint8_t period),
            void (*onLastTwoMinutes)()
        );
        void reset();
        void resetPeriod(bool advancePeriod = false);
        bool isRunning();
        bool isEndOfPeriod();
        void start();
        void stop();
        void prev();
        void next();
        void setHomeScore(uint8_t homeScore);
        void setGuestScore(uint8_t guestScore);
        void decrease();
        void increase();
        void loop();
};

#endif

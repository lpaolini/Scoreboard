#ifndef GameTime_h
#define GameTime_h

#include <Arduino.h>
#include <Adafruit_LEDBackpack.h>
#include "Beeper.h"
#include "Timer.h"
#include "State.h"
#include "Time.h"

class GameTime {
    private:
        const unsigned long MIN = 60000; // one minute
        const unsigned long MAX_TIME = 20 * MIN; // 20 minutes
        const unsigned long preset[7] {3 * MIN, 5 * MIN, 8 * MIN, 10 * MIN, 12 * MIN, 15 * MIN, 20 * MIN};
        const uint8_t defaultPreset = 3;
        const uint8_t START_FLASH_BRIGHTNESS = 15;
        const uint16_t START_FLASH_DURATION_MS = 125;
        const uint16_t STOP_FLASH_DURATION_MS = 250;
        const uint16_t RUN_COLON_FLASH_DURATION_MS = 250;

        Adafruit_7segment *display;
        uint8_t address;
        uint8_t brightness;
        State *state;
        Beeper *beeper;

        void (*onTimeUpdate)(Time time, bool tenths) {};
        void (*onResetPeriod)() {};
        void (*onLastTwoMinutes)() {};

        uint8_t currentPreset = defaultPreset;

        uint8_t homeScore;
        uint8_t guestScore;

        unsigned long time;
        unsigned long adjustedTime;
        unsigned long timeStart;
        unsigned long timeStop;
        bool tenths;

        Time current;
        Time last;

        Timer hold = Timer(1000, false);
        Timer startFlash = Timer(START_FLASH_DURATION_MS, false);

        void setBrightness(uint8_t brightness);
        void enable(bool enabled);
        int decimalDigit(int value, int digit);
        uint8_t presetCount();
        void showTime();
        void showMinSec();
        void showSecTenth();
        void showLastTwoMinutesAlert();
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
        bool isEndOfPeriod();
        bool isEndOfGame();
        void publishTime();
        void loopRun();
        void loopStop();
        void loopGame();
        void loopSetTime();
        void loopSetStep();
        void loopCountdown();

    public:
        GameTime(
            Adafruit_7segment *display,
            uint8_t address,
            uint8_t brightness,
            State *state,
            Beeper *beeper
        );
        void setup(
            void (*onTimeUpdate)(Time time, bool tenths),
            void (*onResetPeriod)(),
            void (*onLastTwoMinutes)()
        );
        void reset();
        void resetPeriod(bool advancePeriod = false);
        void stateChange();
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

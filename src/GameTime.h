#ifndef GameTime_h
#define GameTime_h

#include <Arduino.h>
#include <Adafruit_LEDBackpack.h>
#include "Timer.h"

class GameTime {
    private:
        Adafruit_7segment *display;
        uint8_t address;
        uint8_t brightness;
        void (*onUpdate)(unsigned long time);

        const unsigned long MAX_TIME = 20 * 60000; // 20 minutes
        const unsigned long preset[6] {60000, 180000, 300000, 480000, 600000, 720000};
        const uint8_t defaultPreset = 4;

        uint8_t currentPreset = defaultPreset;

        enum Mode {STOP, RUN, SET_TIME};
        Mode mode = STOP;
        
        bool undoMode = false;

        unsigned long lastTime;
        unsigned long time;
        unsigned long timeStart;
        unsigned long timeStop;

        Timer hold = Timer(1000, false);
        
        int decimalDigit(int value, int digit);
        uint8_t presetCount();
        void update();
        void showMinSec(unsigned long time, bool colon);
        void showSecTenth(unsigned long time);
        void increaseStop(uint8_t delta);
        void increaseSetTime();
        void decreaseStop(uint8_t delta);
        void decreaseSetTime();
        void loopRun();
        void loopStop();
        void loopSetTime();

    public:
        GameTime(
            Adafruit_7segment *display,
            uint8_t address,
            uint8_t brightness,
            void (*onUpdate)(unsigned long remaining)
        );
        void setup();
        void reset();
        void resetPeriod();
        bool isRunning();
        bool isEndOfPeriod();
        void start();
        void stop();
        void toggle();
        void decrease(uint8_t delta);
        void increase(uint8_t delta);
        void loop();
};

#endif

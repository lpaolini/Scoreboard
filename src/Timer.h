#ifndef Timer_h
#define Timer_h

#include <Arduino.h>

class Timer {
    private:
        unsigned long last;
        unsigned long duration;
        bool running;
        bool triggered;
        bool expired;
        bool autoReset;
        bool restarted;

    public:
        explicit Timer(unsigned long duration, bool autoReset = true);
        void stop();
        void reset();
        void reset(unsigned long duration);
        bool isRunning();
        bool isTriggered();
        bool isExpired();
        bool isRestarted();
        unsigned long elapsed();
        void loop();
};

#endif

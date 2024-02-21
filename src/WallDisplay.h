#ifndef WallDisplay_h
#define WallDisplay_h

#include <Arduino.h>

class WallDisplay {
    public:
        virtual void setup();
        virtual void reset();
        virtual void update();
        virtual void setTime(unsigned long time);
        virtual void setHomeScore(uint8_t score);
        virtual void setGuestScore(uint8_t score);
        virtual void setHomeFouls(uint8_t fouls);
        virtual void setGuestFouls(uint8_t fouls);
        virtual void setHomeTimeouts(uint8_t timeouts);
        virtual void setGuestTimeouts(uint8_t timeouts);
        virtual void setBuzzer(bool beeper);
        virtual void loop();
};

#endif

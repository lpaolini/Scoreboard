#ifndef GenericDisplay_h
#define GenericDisplay_h

#include <Arduino.h>
#include <LedControl.h>

class GenericDisplay {
    private:
        LedControl *display;
        uint8_t displayIndex;
        uint8_t brightness;

        uint8_t v1 = 0;
        uint8_t v2 = 0;
        int decimalDigit(int value, int digit);
        void update();

    public:
        GenericDisplay(
            LedControl *display, 
            uint8_t displayIndex,
            uint8_t brightness
        );
        void set(uint8_t v1, uint8_t v2);
        void loop();
};

#endif

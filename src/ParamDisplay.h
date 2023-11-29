#ifndef ParamDisplay_h
#define ParamDisplay_h

#include <Arduino.h>
#include <LedControl.h>
#define PARAM_COUNT 9

class ParamDisplay {
    private:
        LedControl *display;
        uint8_t displayIndex;
        uint8_t brightness;
        void (*onParamsUpdate)(uint8_t (&value)[PARAM_COUNT]);

        int decimalDigit(int value, int digit);
        void update();

        uint8_t field = 0;
        uint8_t maxField = 8;
        uint8_t value[PARAM_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t maxValue[PARAM_COUNT] = {1, 1, 1, 199, 199, 2, 2, 19, 19};

    public:
        ParamDisplay(
            LedControl *display, 
            uint8_t displayIndex,
            uint8_t brightness,
            void (*onParamsUpdate)(uint8_t (&value)[PARAM_COUNT])
        );
        void setup();
        void increaseField();
        void decreaseField();
        void resetField();
        void increaseValue();
        void decreaseValue();
        void resetValue();
};

#endif

#ifndef ScoreButton_h
#define ScoreButton_h

#include <Arduino.h>
#include <LedControl.h>
#include <AceButton.h>
#include "Timer.h"
#include "constants.h"

using namespace ace_button;

class ScoreButton : public IEventHandler {
    private:
        AceButton *button;
        ButtonConfig config;
        uint8_t pin;
        uint16_t pressHoldDelay;
        bool pressed = false;
        void (*onPress)(void) = nullptr;
        void (*onRelease)(void) = nullptr;
        void (*onPressHold)(void) = nullptr;
        void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState);

    public:
        ScoreButton(uint8_t pin, uint16_t pressHoldDelay = 750);
        ScoreButton& setup();
        ScoreButton& press(void (*onPress)(void));
        ScoreButton& release(void (*onRelease)(void));
        ScoreButton& pressHold(void (*onPressHold)(void));
        bool isPressed();
        void loop();
};

#endif

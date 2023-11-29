#ifndef PressHoldButton_h
#define PressHoldButton_h

#include <Arduino.h>
#include <LedControl.h>
#include <AceButton.h>
#include "Timer.h"
#include "constants.h"

using namespace ace_button;

class PressHoldButton : public IEventHandler {
    private:
        AceButton *button;
        ButtonConfig config;
        uint8_t pin;
        uint16_t pressHoldDelay;
        bool longPressRelease;
        bool pressed = false;
        void (*onPress)(void) = nullptr;
        void (*onRelease)(void) = nullptr;
        void (*onPressHold)(void) = nullptr;
        void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState);

    public:
        PressHoldButton(uint8_t pin, uint16_t pressHoldDelay = 750, bool longPressRelease = false);
        PressHoldButton& setup();
        PressHoldButton& press(void (*onPress)(void));
        PressHoldButton& release(void (*onRelease)(void));
        PressHoldButton& pressHold(void (*onPressHold)(void));
        bool isPressed();
        void loop();
};

#endif

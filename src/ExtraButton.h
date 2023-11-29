#ifndef ExtraButton_h
#define ExtraButton_h

#include <Arduino.h>
#include <LedControl.h>
#include <AceButton.h>
#include "Timer.h"
#include "constants.h"

using namespace ace_button;

class ExtraButton : public IEventHandler {
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
        ExtraButton(uint8_t pin, uint16_t pressHoldDelay = 750);
        ExtraButton& setup();
        ExtraButton& press(void (*onPress)(void));
        ExtraButton& release(void (*onRelease)(void));
        ExtraButton& pressHold(void (*onPressHold)(void));
        bool isPressed();
        void loop();
};

#endif

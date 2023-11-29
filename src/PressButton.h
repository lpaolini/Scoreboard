#ifndef PressReleaseButton_h
#define PressReleaseButton_h

#include <Arduino.h>
#include <LedControl.h>
#include <AceButton.h>
#include "Timer.h"
#include "constants.h"

using namespace ace_button;

class PressButton : public IEventHandler {
    private:
        AceButton *button;
        ButtonConfig config;
        uint8_t pin;
        bool pressed = false;
        void (*onPress)(void) = nullptr;
        void (*onRelease)(void) = nullptr;
        void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState);

    public:
        PressButton(uint8_t pin);
        PressButton& setup();
        PressButton& press(void (*onPress)(void));
        PressButton& release(void (*onRelease)(void));
        bool isPressed();
        void loop();
};

#endif

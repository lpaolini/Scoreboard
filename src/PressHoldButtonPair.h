#ifndef PressHoldButtonPair_h
#define PressHoldButtonPair_h

#include <Arduino.h>
#include <LedControl.h>
#include <AceButton.h>
#include "Timer.h"
#include "constants.h"

using namespace ace_button;

class PressHoldButtonPair : public IEventHandler {
    private:
        AceButton *button1;
        AceButton *button2;
        ButtonConfig config;
        uint8_t pin1;
        uint8_t pin2;
        uint16_t pressHoldDelay;
        uint16_t pressHoldRepeatDelay;
        uint16_t pressHoldRepeatInterval;
        bool pressed1 = false;
        bool pressed2 = false;
        bool longPressed1 = false;
        bool longPressed2 = false;
        bool repeatPressed1 = false;
        bool repeatPressed2 = false;
        bool longPressedBoth = false;
        bool repeatDisabled1 = false;
        bool repeatDisabled2 = false;
        void (*onPress1)(void) = nullptr;
        void (*onPressHold1)(void) = nullptr;
        void (*onPressHoldRepeat1)(void) = nullptr;
        void (*onPress2)(void) = nullptr;
        void (*onPressHold2)(void) = nullptr;
        void (*onPressHoldRepeat2)(void) = nullptr;
        void (*onBothPressed)(void) = nullptr;
        void (*onPressHoldBoth)(void) = nullptr;
        void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState);

    public:
        PressHoldButtonPair(uint8_t pin1, uint8_t pin2, uint16_t pressHoldDelay = 3000, uint16_t pressHoldRepeatDelay = 0, uint16_t pressHoldRepeatInterval = 0);
        PressHoldButtonPair& setup();
        PressHoldButtonPair& press1(void (*onPress1)(void));
        PressHoldButtonPair& pressHold1(void (*onPressHold1)(void));
        PressHoldButtonPair& pressHoldRepeat1(void (*onPressHoldRepeat1)(void));
        PressHoldButtonPair& press2(void (*onPress2)(void));
        PressHoldButtonPair& pressHold2(void (*onPressHold2)(void));
        PressHoldButtonPair& pressHoldRepeat2(void (*onPressHoldRepeat2)(void));
        PressHoldButtonPair& pressHoldBoth(void (*onPressHoldBoth)(void));
        void repeatDisable();
        void loop();
};

#endif

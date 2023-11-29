#include "PressHoldButton.h"

PressHoldButton::PressHoldButton(
    uint8_t pin,
    uint16_t pressHoldDelay,
    bool longPressRelease
) {
    this->pin = pin;
    this->pressHoldDelay = pressHoldDelay;
    this->longPressRelease = longPressRelease;
}

PressHoldButton& PressHoldButton::setup() {
    pinMode(pin, INPUT_PULLUP);
    button = new AceButton(&config, pin);
    config.setIEventHandler(this);
    config.setFeature(ButtonConfig::kFeatureLongPress);
    if (!longPressRelease) {
        config.setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
    }
    config.setLongPressDelay(pressHoldDelay);
    return *this;
}

PressHoldButton& PressHoldButton::press(void (*onPress)(void)) {
    this->onPress = onPress;
    return *this;
}

PressHoldButton& PressHoldButton::release(void (*onRelease)(void)) {
    this->onRelease = onRelease;
    return *this;
}

PressHoldButton& PressHoldButton::pressHold(void (*onPressHold)(void)) {
    this->onPressHold = onPressHold;
    return *this;
}

bool PressHoldButton::isPressed() {
    return pressed;
}

void PressHoldButton::handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
    switch (eventType) {
        case AceButton::kEventPressed:
            pressed = true;
            if (onPress != nullptr) {
                onPress();
            }
            break;
        case AceButton::kEventReleased:
            pressed = false;
            if (onRelease != nullptr) {
                onRelease();
            }
            break;
        case AceButton::kEventLongPressed:
            pressed = true;
            if (onPressHold != nullptr) {
                onPressHold();
            }
            break;
        default:
            break;
    }
}

void PressHoldButton::loop() {
    button->check();
}

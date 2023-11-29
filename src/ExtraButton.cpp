#include "ExtraButton.h"

ExtraButton::ExtraButton(
    uint8_t pin,
    uint16_t pressHoldDelay
) {
    this->pin = pin;
    this->pressHoldDelay = pressHoldDelay;
}

ExtraButton& ExtraButton::setup() {
    pinMode(pin, INPUT_PULLUP);
    button = new AceButton(&config, pin);
    config.setIEventHandler(this);
    config.setFeature(ButtonConfig::kFeatureLongPress);
    config.setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
    config.setLongPressDelay(pressHoldDelay);
    return *this;
}

ExtraButton& ExtraButton::press(void (*onPress)(void)) {
    this->onPress = onPress;
    return *this;
}

ExtraButton& ExtraButton::release(void (*onRelease)(void)) {
    this->onRelease = onRelease;
    return *this;
}

ExtraButton& ExtraButton::pressHold(void (*onPressHold)(void)) {
    this->onPressHold = onPressHold;
    return *this;
}

bool ExtraButton::isPressed() {
    return pressed;
}

void ExtraButton::handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
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

void ExtraButton::loop() {
    button->check();
}

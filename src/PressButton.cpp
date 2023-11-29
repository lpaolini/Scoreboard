#include "PressButton.h"

PressButton::PressButton(
    uint8_t pin
) {
    this->pin = pin;
}

PressButton& PressButton::setup() {
    pinMode(pin, INPUT_PULLUP);
    button = new AceButton(&config, pin);
    config.setIEventHandler(this);
    return *this;
}

PressButton& PressButton::press(void (*onPress)(void)) {
    this->onPress = onPress;
    return *this;
}

PressButton& PressButton::release(void (*onRelease)(void)) {
    this->onRelease = onRelease;
    return *this;
}

bool PressButton::isPressed() {
    return pressed;
}

void PressButton::handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
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
        default:
            break;
    }
}

void PressButton::loop() {
    button->check();
}

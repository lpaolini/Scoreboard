#include "ScoreButton.h"

ScoreButton::ScoreButton(
    uint8_t pin,
    uint16_t pressHoldDelay
) {
    this->pin = pin;
    this->pressHoldDelay = pressHoldDelay;
}

ScoreButton& ScoreButton::setup() {
    pinMode(pin, INPUT_PULLUP);
    button = new AceButton(&config, pin);
    config.setIEventHandler(this);
    config.setFeature(ButtonConfig::kFeatureLongPress);
    config.setLongPressDelay(pressHoldDelay);
    return *this;
}

ScoreButton& ScoreButton::press(void (*onPress)(void)) {
    this->onPress = onPress;
    return *this;
}

ScoreButton& ScoreButton::release(void (*onRelease)(void)) {
    this->onRelease = onRelease;
    return *this;
}

ScoreButton& ScoreButton::pressHold(void (*onPressHold)(void)) {
    this->onPressHold = onPressHold;
    return *this;
}

bool ScoreButton::isPressed() {
    return pressed;
}

void ScoreButton::handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
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
            if (onPressHold != nullptr) {
                onPressHold();
            }
            break;
        default:
            break;
    }
}

void ScoreButton::loop() {
    button->check();
}

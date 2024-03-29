#include "PressHoldButtonPair.h"

PressHoldButtonPair::PressHoldButtonPair(
    uint8_t pin1,
    uint8_t pin2,
    uint16_t pressHoldDelay,
    uint16_t pressHoldRepeatDelay,
    uint16_t pressHoldRepeatMaxInterval,
    uint16_t pressHoldRepeatMinInterval
) {
    this->pin1 = pin1;
    this->pin2 = pin2;
    this->pressHoldDelay = pressHoldDelay;
    this->pressHoldRepeatDelay = pressHoldRepeatDelay;
    this->pressHoldRepeatMaxInterval = pressHoldRepeatMaxInterval;
    this->pressHoldRepeatMinInterval = pressHoldRepeatMinInterval;
    this->pressHoldRepeatInterval = pressHoldRepeatMaxInterval;
}

PressHoldButtonPair& PressHoldButtonPair::setup() {
    pinMode(pin1, INPUT_PULLUP);
    pinMode(pin2, INPUT_PULLUP);
    button1 = new AceButton(&config, pin1);
    button2 = new AceButton(&config, pin2);
    config.setIEventHandler(this);
    config.setFeature(ButtonConfig::kFeatureLongPress);
    config.setFeature(ButtonConfig::kFeatureRepeatPress);
    config.setLongPressDelay(pressHoldDelay);
    config.setRepeatPressDelay(pressHoldRepeatDelay);
    config.setRepeatPressInterval(pressHoldRepeatInterval);
    return *this;
}

PressHoldButtonPair& PressHoldButtonPair::press1(void (*onPress1)(void)) {
    this->onPress1 = onPress1;
    return *this;
}

PressHoldButtonPair& PressHoldButtonPair::pressHold1(void (*onPressHold1)(void)) {
    this->onPressHold1 = onPressHold1;
    return *this;
}

PressHoldButtonPair& PressHoldButtonPair::pressHoldRepeat1(void (*onPressHoldRepeat1)(void)) {
    this->onPressHoldRepeat1 = onPressHoldRepeat1;
    return *this;
}

PressHoldButtonPair& PressHoldButtonPair::press2(void (*onPress2)(void)) {
    this->onPress2 = onPress2;
    return *this;
}

PressHoldButtonPair& PressHoldButtonPair::pressHold2(void (*onPressHold2)(void)) {
    this->onPressHold2 = onPressHold2;
    return *this;
}

PressHoldButtonPair& PressHoldButtonPair::pressHoldRepeat2(void (*onPressHoldRepeat2)(void)) {
    this->onPressHoldRepeat2 = onPressHoldRepeat2;
    return *this;
}

PressHoldButtonPair& PressHoldButtonPair::pressBoth(void (*onPressBoth)(bool pressed)) {
    this->onPressBoth = onPressBoth;
    return *this;
}

PressHoldButtonPair& PressHoldButtonPair::pressHoldBoth(void (*onPressHoldBoth)(void)) {
    this->onPressHoldBoth = onPressHoldBoth;
    return *this;
}

void PressHoldButtonPair::handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
    uint8_t pin = button->getPin();
    switch (eventType) {
        case AceButton::kEventPressed:
            if (pin == pin1) {
                pressed1 = true;
                if (onPress1 != nullptr) {
                    onPress1();
                }
                if (pressed2 && onPressBoth != nullptr) {
                    onPressBoth(true);
                }
            } else if (pin == pin2) {
                pressed2 = true;
                if (onPress2 != nullptr) {
                    onPress2();
                }
                if (pressed1 && onPressBoth != nullptr) {
                    onPressBoth(true);
                }
            }
            break;
        case AceButton::kEventLongPressed:
            if (pin == pin1) {
                longPressed1 = true;
                if (longPressed2 && onPressHoldBoth != nullptr) {
                    longPressedBoth = true;
                    onPressHoldBoth();
                } else if (onPressHold1 != nullptr) {
                    onPressHold1();
                }
            } else if (pin == pin2) {
                longPressed2 = true;
                if (longPressed1 && onPressHoldBoth != nullptr) {
                    longPressedBoth = true;
                    onPressHoldBoth();
                } else if (onPressHold2 != nullptr) {
                    onPressHold2();
                }
            }
            break;
        case AceButton::kEventRepeatPressed:
            if (pin == pin1) {
                repeatPressed1 = true;
                if (pressed2 || longPressedBoth) {
                    repeatDisabled1 = true;
                }
                if (!repeatDisabled1 && onPressHoldRepeat1 != nullptr) {
                    decreaseRepeatInterval();
                    onPressHoldRepeat1();
                }
            } else if (pin == pin2) {
                repeatPressed2 = true;
                if (pressed1 || longPressedBoth) {
                    repeatDisabled2 = true;
                }
                if (!repeatDisabled2 && onPressHoldRepeat2 != nullptr) {
                    decreaseRepeatInterval();
                    onPressHoldRepeat2();
                }
            }
            break;
        case AceButton::kEventReleased:
            if (pin == pin1) {
                pressed1 = false;
                longPressed1 = false;
                repeatPressed1 = false;
                repeatDisabled1 = false;
                longPressedBoth = false;
                if (pressed2 && onPressBoth != nullptr) {
                    onPressBoth(false);
                }
                resetRepeatInterval();
            } else if (pin == pin2) {
                pressed2 = false;
                longPressed2 = false;
                repeatPressed2 = false;
                repeatDisabled2 = false;
                longPressedBoth = false;
                if (pressed1 && onPressBoth != nullptr) {
                    onPressBoth(false);
                }
                resetRepeatInterval();
            }
            break;
        default:
            break;
    }
}

bool PressHoldButtonPair::isPressed1() {
    return pressed1;
}

bool PressHoldButtonPair::isPressed2() {
    return pressed2;
}

void PressHoldButtonPair::repeatDisable() {
    if (repeatPressed1) {
        repeatDisabled1 = true;
    }
    if (repeatPressed2) {
        repeatDisabled2 = true;
    }
}

void PressHoldButtonPair::decreaseRepeatInterval() {
    if (pressHoldRepeatInterval > pressHoldRepeatMinInterval) {
        pressHoldRepeatInterval--;
        config.setRepeatPressInterval(pressHoldRepeatInterval);
    }
}

void PressHoldButtonPair::resetRepeatInterval() {
    pressHoldRepeatInterval = pressHoldRepeatMaxInterval;
    config.setRepeatPressInterval(pressHoldRepeatInterval);
}

void PressHoldButtonPair::loop() {
    button1->check();
    button2->check();
}

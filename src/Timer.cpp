#include "Timer.h"

Timer::Timer(unsigned long duration, bool autoReset) {
    this->duration = duration;
    this->autoReset = autoReset;
    stop();
}

void Timer::stop(bool trigger) {
    running = false;
    last = millis();
    triggered = trigger;
    expired = false;
}

void Timer::reset() {
    running = true;
    triggered = false;
    expired = false;
    last = millis();
}

void Timer::reset(unsigned long duration) {
    this->duration = duration;
    reset();
}

bool Timer::isRunning() {
    return running;
}

bool Timer::isTriggered() {
    if (triggered) {
        triggered = false;
        return true;
    }
    return false;
}

bool Timer::isExpired() {
    return expired;
}

bool Timer::isRestarted() {
    if (restarted) {
        restarted = false;
        return true;
    }
    return false;
}

unsigned long Timer::elapsed() {
    if (running) {
        unsigned long current = millis();
        return current - last;
    } else {
        return 0;
    }
}

void Timer::loop() {
    if (running) {
        expired = false;
        restarted = false;
        unsigned long current = millis();
        if (current - last >= duration) {
            if (autoReset) {
                last += duration;
                restarted = true;
                if (current - last > duration) {
                    last = current;
                }
            } else {
                running = false;
                expired = true;
            }
            triggered = true;
        }
    }
}

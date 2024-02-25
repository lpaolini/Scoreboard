#include <Arduino.h>
#include <LedControl.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <TimerOne.h>

#include "constants.h"

#include "State.h"
#include "Beeper.h"

#include "ElvasDisplay.h"
#include "Score.h"
#include "Extra.h"
#include "GameTime.h"

#include "PressButton.h"
#include "PressHoldButton.h"
#include "PressHoldButtonPair.h"
#include "ScoreButton.h"
#include "ExtraButton.h"

// state

State *state = new State();

// beeper

Beeper *beeper = new Beeper(BUZZER_PIN);

// displays

WallDisplay *wallDisplay = new ElvasDisplay(ELVAS_PIN, LED_PIN, state);
LedControl *displayBus = new LedControl(SPI_DISPLAY_DATA_PIN, SPI_DISPLAY_CLK_PIN, SPI_DISPLAY_CS_PIN, 4);
Adafruit_7segment *timeDisplay = new Adafruit_7segment();

// controllers

Extra *homeExtra = new Extra(displayBus, 0, BRIGHTNESS, false, state, beeper);
Extra *guestExtra = new Extra(displayBus, 1, BRIGHTNESS, true, state, beeper);
Score *homeScore = new Score(displayBus, 2, BRIGHTNESS, true, state, beeper);
Score *guestScore = new Score(displayBus, 3, BRIGHTNESS, false, state, beeper);
GameTime *gameTime = new GameTime(timeDisplay, TIME_DISPLAY_ADDR, BRIGHTNESS, state, beeper);

// buttons

ScoreButton homeScoreButton(HOME_SCORE_BUTTON_PIN, 750);
ScoreButton guestScoreButton(GUEST_SCORE_BUTTON_PIN, 750);
ExtraButton homeExtraButton(HOME_EXTRA_BUTTON_PIN, 750);
ExtraButton guestExtraButton(GUEST_EXTRA_BUTTON_PIN, 750);
PressButton timeButton(TIME_BUTTON_PIN);
PressButton undoButton(UNDO_BUTTON_PIN);
PressButton buzzerButton(BUZZER_BUTTON_PIN);
PressHoldButtonPair adjustButtons(MINUS_BUTTON_PIN, PLUS_BUTTON_PIN, 2000, 500, 75, 10);

// wiring

void onHomeFoulsUpdate(uint8_t fouls) {
    wallDisplay->setHomeFouls(fouls);
}

void onHomeTimeoutsUpdate(uint8_t timeouts) {
    wallDisplay->setHomeTimeouts(timeouts);
}

void onGuestFoulsUpdate(uint8_t fouls) {
    wallDisplay->setGuestFouls(fouls);
}

void onGuestTimeoutsUpdate(uint8_t timeouts) {
    wallDisplay->setGuestTimeouts(timeouts);
}

void onTimeUpdate(unsigned long time) {
    wallDisplay->setTime(time);
}

void onGameMode(bool gameMode) {
    if (!gameMode) {
        homeScore->enable(false);
        guestScore->enable(false);
        homeExtra->enable(false);
        guestExtra->enable(false);
    }
}

void onStateChange() {
    wallDisplay->stateChange();
}

void onResetPeriod() {
    homeScore->resetPeriod();
    guestScore->resetPeriod();
    homeExtra->resetPeriod();
    guestExtra->resetPeriod();
}

void onLastTwoMinutes() {
    homeExtra->lastTwoMinutes();
    guestExtra->lastTwoMinutes();
    beeper->lastTwoMinutes();
}

void onHomeScoreUpdate(uint8_t score) {
    wallDisplay->setHomeScore(score);
    gameTime->setHomeScore(score);
}

void onGuestScoreUpdate(uint8_t score) {
    wallDisplay->setGuestScore(score);
    gameTime->setGuestScore(score);
}

void setupWallDisplay() {
    wallDisplay->setup();
}

void loopWallDisplay() {
    wallDisplay->loop();
}

void setupControllers() {
    state->setOnUpdate(onStateChange);
    gameTime->setup(onTimeUpdate, onGameMode, onResetPeriod, onLastTwoMinutes);
    homeScore->setup(onHomeScoreUpdate);
    homeExtra->setup(onHomeFoulsUpdate, onHomeTimeoutsUpdate);
    guestScore->setup(onGuestScoreUpdate);
    guestExtra->setup(onGuestFoulsUpdate, onGuestTimeoutsUpdate);
}

void setupBeeper() {
    beeper->setup();
}

void loopBuzzer() {
    beeper->loop();
}

void loopControllers() {
    gameTime->loop();
    homeScore->loop();
    guestScore->loop();
    homeExtra->loop();
    guestExtra->loop();
}

void hardReset() {
    asm volatile ("jmp 0x7800");
}

void softReset() {
    homeScore->reset();
    guestScore->reset();
    homeExtra->reset();
    guestExtra->reset();
    gameTime->reset();
    wallDisplay->reset();
    beeper->ready();
}

void reset() {
    if (undoButton.isPressed()) {
        gameTime->enable(false);
        homeScore->enable(false);
        guestScore->enable(false);
        homeExtra->enable(false);
        guestExtra->enable(false);
        beeper->confirm([] {
            hardReset();
            // softReset();
        });
    } else {
        gameTime->resetPeriod(false);
    }
}

void adjustIncrease(bool repeat) {
    if (homeScoreButton.isPressed() || guestScoreButton.isPressed()) {
        if (homeScoreButton.isPressed()) {
            homeScore->increaseScore();
        } 
        if (guestScoreButton.isPressed()) {
            guestScore->increaseScore();
        } 
    } else {
        gameTime->increase();
    }
}

void adjustDecrease(bool repeat) {
    if (homeScoreButton.isPressed() || guestScoreButton.isPressed()) {
        if (homeScoreButton.isPressed()) {
            homeScore->decreaseScore();
        } 
        if (guestScoreButton.isPressed()) {
            guestScore->decreaseScore();
        } 
    } else {
        gameTime->decrease();
    }
}

void setupButtons() {
    homeScoreButton.setup()
        .press([] {
            if (undoButton.isPressed()) {
                beeper->click();
                // adjustButtons.repeatDisable();
                // homeScore->decreaseDelta();
                homeScore->undo();
            } else {
                beeper->click();
                adjustButtons.repeatDisable(); 
                homeScore->increaseDelta();
            }
        })
        .release([] {
            adjustButtons.repeatDisable();
            homeScore->startTimer();
        })
        .pressHold([] {
            homeScore->clearDelta();
        });

    guestScoreButton.setup()
        .press([] {
            if (undoButton.isPressed()) {
                beeper->click();
                // adjustButtons.repeatDisable();
                // guestScore->decreaseDelta();
                guestScore->undo();
            } else {
                beeper->click();
                adjustButtons.repeatDisable(); 
                guestScore->increaseDelta();
            }
        })
        .release([] {
            adjustButtons.repeatDisable();
            guestScore->startTimer();
        })
        .pressHold([] {
            guestScore->clearDelta();
        });

    homeExtraButton.setup()
        .press([] {
            beeper->click();
        })
        .release([] {
            if (undoButton.isPressed()) {
                homeExtra->decreaseFouls();
            } else {
                homeExtra->increaseFouls();
            }
        })
        .pressHold([] {
            if (undoButton.isPressed()) {
                homeExtra->decreaseTimeouts();
            } else {
                homeExtra->increaseTimeouts();
            }
        });

    guestExtraButton.setup()
        .press([] {
            beeper->click();
        })
        .release([] {
            if (undoButton.isPressed()) {
                guestExtra->decreaseFouls();
            } else {
                guestExtra->increaseFouls();
            }
        })
        .pressHold([] {
            if (undoButton.isPressed()) {
                guestExtra->decreaseTimeouts();
            } else {
                guestExtra->increaseTimeouts();
            }
        });

    timeButton.setup()
        .press([] {
            beeper->click();
            if (undoButton.isPressed()) {
                gameTime->prev();
            } else {
                gameTime->next(); 
            }
        });

    adjustButtons.setup()
        .press1([] {
            beeper->click();
            adjustIncrease(false);
        })
        .pressHoldRepeat1([] {
            adjustIncrease(true);
        })
        .press2([] {
            beeper->click();
            adjustDecrease(false);
        })
        .pressHoldRepeat2([] {
            adjustDecrease(true);
        })
        .pressBoth([](bool pressed) {
            if (pressed && !gameTime->isRunning()) {
                beeper->alert(true);
            } else {
                beeper->alert(false);
            }
        })
        .pressHoldBoth([] { 
            if (!gameTime->isRunning()) {
                reset();
            }
        });

    undoButton.setup()
        .press([] {
            beeper->click();
        });

    buzzerButton.setup()
        .press([] {
            beeper->click();
            wallDisplay->setBuzzer(true);
        })
        .release([] { wallDisplay->setBuzzer(false); });
}

void loopButtons() {
    homeScoreButton.loop();
    guestScoreButton.loop();
    homeExtraButton.loop();
    guestExtraButton.loop();
    timeButton.loop();
    adjustButtons.loop();
    undoButton.loop();
    buzzerButton.loop();
}

void setupTimer() {
    Timer1.initialize(BIT_DURATION_MICROSECONDS);
    Timer1.attachInterrupt([] { wallDisplay->update(); });
    Timer1.start();
}

void setup() {
    Serial.begin(230400); 
    setupControllers();
    setupBeeper();
    setupButtons();
    setupWallDisplay();
    setupTimer();
    beeper->ready();
}

void loop() {
    loopControllers();
    loopBuzzer();
    loopButtons();
    loopWallDisplay();
}

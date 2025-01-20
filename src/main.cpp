#include <Arduino.h>
#include <LedControl.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <TimerOne.h>

#include "constants.h"

#include "State.h"
#include "Time.h"
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

// State

State *state = new State();

// Beeper

Beeper *beeper = new Beeper(BUZZER_PIN);

// Displays

WallDisplay *wallDisplay = new ElvasDisplay(ELVAS_PIN, LED_PIN, state);
LedControl *displayBus = new LedControl(SPI_DISPLAY_DATA_PIN, SPI_DISPLAY_CLK_PIN, SPI_DISPLAY_CS_PIN, 4);
Adafruit_7segment *timeDisplay = new Adafruit_7segment();

// Controllers

Extra *homeExtra = new Extra(displayBus, HOME_EXTRA_DISPLAY_ADDR, BRIGHTNESS, false, state, beeper);
Extra *guestExtra = new Extra(displayBus, GUEST_EXTRA_DISPLAY_ADDR, BRIGHTNESS, true, state, beeper);
Score *homeScore = new Score(displayBus, HOME_SCORE_DISPLAY_ADDR, BRIGHTNESS, true, state, beeper);
Score *guestScore = new Score(displayBus, GUEST_SCORE_DISPLAY_ADDR, BRIGHTNESS, false, state, beeper);
GameTime *gameTime = new GameTime(timeDisplay, TIME_DISPLAY_ADDR, BRIGHTNESS, state, beeper);

// Buttons

ScoreButton homeScoreButton(HOME_SCORE_BUTTON_PIN, 750);
ScoreButton guestScoreButton(GUEST_SCORE_BUTTON_PIN, 750);
ExtraButton homeExtraButton(HOME_EXTRA_BUTTON_PIN, 750);
ExtraButton guestExtraButton(GUEST_EXTRA_BUTTON_PIN, 750);
PressButton timeButton(TIME_BUTTON_PIN);
PressButton undoButton(UNDO_BUTTON_PIN);
PressButton buzzerButton(BUZZER_BUTTON_PIN);
PressHoldButtonPair adjustButtons(MINUS_BUTTON_PIN, PLUS_BUTTON_PIN, 2000, 500, 75, 10);

// Wiring

void onHomeFoulsUpdate(uint8_t fouls) {
    wallDisplay->setHomeFouls(fouls);
}

void onHomeTimeoutsUpdate(uint8_t timeouts, bool increased) {
    wallDisplay->setHomeTimeouts(timeouts);
    if (increased && state->getChrono() == STOP) {
        gameTime->startTimeout();
    } 
}

void onGuestFoulsUpdate(uint8_t fouls) {
    wallDisplay->setGuestFouls(fouls);
}

void onGuestTimeoutsUpdate(uint8_t timeouts, bool increased) {
    wallDisplay->setGuestTimeouts(timeouts);
    if (increased && state->getChrono() == STOP) {
        gameTime->startTimeout();
    } 
}

void onTimeUpdate(Time time, bool tenths) {
    wallDisplay->setTime(time, tenths);
}

void showSelectPeriod() {
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 7, ' ', false);
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 6, ' ', false);
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 5, 'S', false);
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 4, 'C', false);
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 3, 'E', false);
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 2, 'G', false);
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 1, 'L', false);
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 0, 'I', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 7, 'P', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 6, 'E', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 5, 'r', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 4, 'i', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 3, 'o', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 2, 'd', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 1, 'o', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 0, ' ', false);
}

void showSelectDuration() {
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 7, ' ', false);
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 6, ' ', false);
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 5, 'S', false);
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 4, 'C', false);
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 3, 'E', false);
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 2, 'G', false);
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 1, 'L', false);
    displayBus->setChar(HOME_EXTRA_DISPLAY_ADDR, 0, 'I', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 7, 'd', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 6, 'u', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 5, 'r', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 4, 'A', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 3, 't', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 2, 'A', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 1, ' ', false);
    displayBus->setChar(GUEST_EXTRA_DISPLAY_ADDR, 0, ' ', false);
}

void onStateChange() {
    wallDisplay->stateChange();
    homeScore->stateChange();
    guestScore->stateChange();
    homeExtra->stateChange();
    guestExtra->stateChange();
    gameTime->stateChange();
    if (state->getMode() == SET_STEP) {
        showSelectPeriod();
    }            
    if (state->getMode() == SET_TIME) {
        showSelectDuration();
    }
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
}

void onHomeScoreUpdate(uint8_t score) {
    wallDisplay->setHomeScore(score);
    gameTime->setHomeScore(score);
}

void onGuestScoreUpdate(uint8_t score) {
    wallDisplay->setGuestScore(score);
    gameTime->setGuestScore(score);
}

void onEndOfPeriod() {
    wallDisplay->endOfPeriod();
}

void onEndOfTimeout() {
    wallDisplay->endOfTimeout();
}

// Reset

void reset() {
    wallDisplay->reset();
    homeScore->reset();
    guestScore->reset();
    homeExtra->reset();
    guestExtra->reset();
    gameTime->reset();
}

// Controllers

void setupControllers() {
    state->setup(onStateChange);
    gameTime->setup(onTimeUpdate, onResetPeriod, onLastTwoMinutes, onEndOfPeriod, onEndOfTimeout);
    homeScore->setup(onHomeScoreUpdate);
    homeExtra->setup(onHomeFoulsUpdate, onHomeTimeoutsUpdate);
    guestScore->setup(onGuestScoreUpdate);
    guestExtra->setup(onGuestFoulsUpdate, onGuestTimeoutsUpdate);
}

void loopControllers() {
    gameTime->loop();
    homeScore->loop();
    guestScore->loop();
    homeExtra->loop();
    guestExtra->loop();
}

// Buttons

void adjustIncrease() {
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

void adjustDecrease() {
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
            if (homeScore->isEnabled()) {
                beeper->click();
                if (undoButton.isPressed()) {
                    adjustButtons.repeatDisable();
                    homeScore->decreaseDelta();
                    // homeScore->undo();
                } else {
                    adjustButtons.repeatDisable(); 
                    homeScore->increaseDelta();
                }
            } else {
                beeper->notAllowed();
            }
        })
        .release([] {
            adjustButtons.repeatDisable();
            homeScore->startTimer();
            homeScore->alterScore(false);
        })
        .pressHold([] {
            homeScore->alterScore(true);
        });

    guestScoreButton.setup()
        .press([] {
            if (guestScore->isEnabled()) {
                beeper->click();
                if (undoButton.isPressed()) {
                    adjustButtons.repeatDisable();
                    guestScore->decreaseDelta();
                    // guestScore->undo();
                } else {
                    adjustButtons.repeatDisable(); 
                    guestScore->increaseDelta();
                }
            } else {
                beeper->notAllowed();
            }
        })
        .release([] {
            adjustButtons.repeatDisable();
            guestScore->startTimer();
            guestScore->alterScore(false);
        })
        .pressHold([] {
            guestScore->alterScore(true);
        });

    homeExtraButton.setup()
        .press([] {
            if (homeExtra->isEnabled()) {
                beeper->click();
            } else {
                beeper->notAllowed();
            }
        })
        .release([] {
            if (undoButton.isPressed()) {
                homeExtra->decreaseFouls();
            } else {
                homeExtra->increaseFouls();
            }
        })
        .pressHold([] {
            gameTime->stopTimeout();
            if (undoButton.isPressed()) {
                homeExtra->decreaseTimeouts();
            } else {
                homeExtra->increaseTimeouts();
            }
        });

    guestExtraButton.setup()
        .press([] {
            if (guestExtra->isEnabled()) {
                beeper->click();
            } else {
                beeper->notAllowed();
            }
        })
        .release([] {
            if (undoButton.isPressed()) {
                guestExtra->decreaseFouls();
            } else {
                guestExtra->increaseFouls();
            }
        })
        .pressHold([] {
            gameTime->stopTimeout();
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
            adjustIncrease();
        })
        .pressHoldRepeat1([] {
            adjustIncrease();
        })
        .press2([] {
            beeper->click();
            adjustDecrease();
        })
        .pressHoldRepeat2([] {
            adjustDecrease();
        })
        .pressBoth([](bool pressed) {
            if (pressed) {
                if (state->getChrono() == STOP) {
                    beeper->alert(true);
                } else {
                    beeper->notAllowed();
                }
            } else {
                beeper->alert(false);
            }
        })
        .pressHoldBoth([] { 
            if (state->getChrono() == STOP) {
                if (undoButton.isPressed()) {
                    reset();
                } else {
                    gameTime->resetPeriod(false);
                }
            }
        });

    undoButton.setup()
        .press([] {
            if (!adjustButtons.isPressed1() || !adjustButtons.isPressed2()) {
                beeper->click();
            }
        });

    buzzerButton.setup()
        .press([] {
            beeper->click();
            wallDisplay->setBuzzer(true);
        })
        .release([] {
            wallDisplay->setBuzzer(false);
        });
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

// Timer

void setupTimer() {
    Timer1.initialize(BIT_DURATION_MICROSECONDS);
    Timer1.attachInterrupt([] { wallDisplay->update(); });
    Timer1.start();
}

// Main 

void setup() {
    Serial.begin(230400);
    while (!Serial) {}
    beeper->setup();
    wallDisplay->setup();
    setupControllers();
    setupButtons();
    setupTimer();
    reset();
}

void loop() {
    beeper->loop();
    wallDisplay->loop();
    loopControllers();
    loopButtons();
}

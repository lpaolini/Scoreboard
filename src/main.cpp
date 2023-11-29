#include <Arduino.h>
#include <LedControl.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

#include <TimerOne.h>
// #include "TimerOneEx.h"

#include "ElvasDisplay.h"
#include "Score.h"
#include "Extra.h"
#include "GameTime.h"
#include "ParamDisplay.h"

#include "PressButton.h"
#include "PressHoldButton.h"
#include "PressHoldButtonPair.h"
#include "ScoreButton.h"
#include "ExtraButton.h"

// Wall panel

#define ELVAS_PIN 9

// Displays

#define SPI_DISPLAY_CS_PIN 10
#define SPI_DISPLAY_CLK_PIN 11
#define SPI_DISPLAY_DATA_PIN 12
#define TIME_DISPLAY_ADDR 0x70
#define BRIGHTNESS 1

// Buttons

#define J3 8
#define J4 7
#define J5 6
#define J6 3
#define J7 4
#define J8 5
#define J9 14
#define J10 15
#define J11 16
#define J12 17

#define HOME_SCORE_BUTTON_PIN J3
#define HOME_EXTRA_BUTTON_PIN J4
#define GUEST_SCORE_BUTTON_PIN J6
#define GUEST_EXTRA_BUTTON_PIN J7
#define TIME_BUTTON_PIN J8
#define MINUS_BUTTON_PIN J12
#define PLUS_BUTTON_PIN J11
#define BUZZER_BUTTON_PIN J9
#define UNDO_BUTTON_PIN J10

// TimerOneEx<Elvas> timer;

ElvasDisplay *elvas = new ElvasDisplay(ELVAS_PIN);
LedControl *displayBus = new LedControl(SPI_DISPLAY_DATA_PIN, SPI_DISPLAY_CLK_PIN, SPI_DISPLAY_CS_PIN, 4);
Adafruit_7segment *timeDisplay = new Adafruit_7segment();

void onTimeUpdate(unsigned long time) {
    elvas->setTime(time);
}

void onHomeScoreUpdate(uint8_t score) {
    elvas->setHomeScore(score);
}

void onGuestScoreUpdate(uint8_t score) {
    elvas->setGuestScore(score);
}

void onParamUpdate(uint8_t (&value)[9]) {
    elvas->setUnknown1(value[0]);
    elvas->setUnknown2(value[1]);
    elvas->setBuzzer(value[2]);
    elvas->setHomeScore(value[3]);
    elvas->setGuestScore(value[4]);
    elvas->setHomeTimeouts(value[5]);
    elvas->setGuestTimeouts(value[6]);
    elvas->setHomeFouls(value[7]);
    elvas->setGuestFouls(value[8]);
}

void onHomeFoulsUpdate(uint8_t fouls) {
    elvas->setHomeFouls(fouls);
}

void onHomeTimeoutsUpdate(uint8_t timeouts) {
    elvas->setHomeTimeouts(timeouts);
}

void onGuestFoulsUpdate(uint8_t fouls) {
    elvas->setGuestFouls(fouls);
}

void onGuestTimeoutsUpdate(uint8_t timeouts) {
    elvas->setGuestTimeouts(timeouts);
}

Score *homeScore = new Score(displayBus, 2, BRIGHTNESS, true, onHomeScoreUpdate);
Score *guestScore = new Score(displayBus, 3, BRIGHTNESS, false, onGuestScoreUpdate);
Extra *homeExtra = new Extra(displayBus, 0, BRIGHTNESS, false, onHomeFoulsUpdate, onHomeTimeoutsUpdate);
Extra *guestExtra = new Extra(displayBus, 1, BRIGHTNESS, true, onGuestFoulsUpdate, onGuestTimeoutsUpdate);
GameTime *gameTime = new GameTime(timeDisplay, TIME_DISPLAY_ADDR, BRIGHTNESS, onTimeUpdate);

// PressHoldButton homeScoreButton(HOME_SCORE_BUTTON_PIN, 750, true);
// PressHoldButton guestScoreButton(GUEST_SCORE_BUTTON_PIN, 750, true);
// PressHoldButton homeExtraButton(HOME_EXTRA_BUTTON_PIN, 750, false);
// PressHoldButton guestExtraButton(GUEST_EXTRA_BUTTON_PIN, 750, false);
ScoreButton homeScoreButton(HOME_SCORE_BUTTON_PIN, 750);
ScoreButton guestScoreButton(GUEST_SCORE_BUTTON_PIN, 750);
ExtraButton homeExtraButton(HOME_EXTRA_BUTTON_PIN, 750);
ExtraButton guestExtraButton(GUEST_EXTRA_BUTTON_PIN, 750);

PressButton timeButton(TIME_BUTTON_PIN);
PressHoldButtonPair adjustButtons(MINUS_BUTTON_PIN, PLUS_BUTTON_PIN, 2000, 500, 100);
PressButton undoButton(UNDO_BUTTON_PIN);
PressButton buzzerButton(BUZZER_BUTTON_PIN);

void setupWallDisplay() {
    elvas->setup();
}

void loopWallDisplay() {
    elvas->loop();
}

void setupControllers() {
    gameTime->setup();
    homeScore->setup();
    homeExtra->setup();
    guestScore->setup();
    guestExtra->setup();
}

void loopControllers() {
    gameTime->loop();
    homeScore->loop();
    guestScore->loop();
    homeExtra->loop();
    guestExtra->loop();
}

void resetPeriod() {
    homeExtra->resetPeriod();
    guestExtra->resetPeriod();
    gameTime->resetPeriod();
}

void reset() {
    if (undoButton.isPressed()) {
        homeScore->reset();
        guestScore->reset();
        homeExtra->reset();
        guestExtra->reset();
        gameTime->reset();
    } else {
        resetPeriod();
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
    } else if (!gameTime->isRunning()) {
        gameTime->increase(1);
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
    } else if (!gameTime->isRunning()) {
        gameTime->decrease(1);
    }
}

void setupButtons() {
    homeScoreButton.setup()
        .press([] {
            if (undoButton.isPressed()) {
                homeScore->undo();
            } else {
                adjustButtons.repeatDisable(); 
                homeScore->nextDelta();
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
                guestScore->undo();
            } else {
                adjustButtons.repeatDisable(); 
                guestScore->nextDelta();
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
            if (gameTime->isEndOfPeriod()) {
                resetPeriod();
            } else {
                gameTime->toggle(); 
            }
        });

    adjustButtons.setup()
        .press1([] {
            adjustIncrease(false);
        })
        .pressHoldRepeat1([] {
            adjustIncrease(true);
        })
        .press2([] {
            adjustDecrease(false);
        })
        .pressHoldRepeat2([] {
            adjustDecrease(true);
        })
        .pressHoldBoth([] { 
            if (!gameTime->isRunning()) {
                reset();
            }
        });

    undoButton.setup();

    buzzerButton.setup()
        .press([] { elvas->setBuzzer(true); })
        .release([] { elvas->setBuzzer(false); });
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
    Timer1.attachInterrupt([] { elvas->update(); });
    Timer1.start();
}

void setup() {
    Serial.begin(115200); 
    setupControllers();
    setupButtons();
    setupWallDisplay();
    setupTimer();
}

void loop() {
    loopControllers();
    loopButtons();
    loopWallDisplay();
}

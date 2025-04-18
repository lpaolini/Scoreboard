#ifndef constants_h
#define constants_h

#define ELVAS_DEBUG
// #define STATE_DEBUG
#define SOUND

// Wall panel

#define MAX_SCORE 199
#define MAX_FOULS 19
#define MAX_WALL_DISPLAY_FOULS 5
#define MAX_TIMEOUTS 3
#define ELVAS_PIN 9
#define LED_PIN 13

// Board displays

#define SPI_DISPLAY_CS_PIN 10
#define SPI_DISPLAY_CLK_PIN 11
#define SPI_DISPLAY_DATA_PIN 12
#define TIME_DISPLAY_ADDR 0x70
#define BRIGHTNESS 8
#define BOTTOM_LEFT_DISPLAY 0
#define BOTTOM_RIGHT_DISPLAY 1
#define TOP_LEFT_DISPLAY 2
#define TOP_RIGHT_DISPLAY 3

// Board connectors

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

// Displays

#define HOME_EXTRA_DISPLAY_ADDR BOTTOM_LEFT_DISPLAY
#define GUEST_EXTRA_DISPLAY_ADDR BOTTOM_RIGHT_DISPLAY
#define HOME_SCORE_DISPLAY_ADDR TOP_LEFT_DISPLAY
#define GUEST_SCORE_DISPLAY_ADDR TOP_RIGHT_DISPLAY

// Buttons

#define HOME_SCORE_BUTTON_PIN J3
#define HOME_EXTRA_BUTTON_PIN J4
#define BUZZER_BUTTON_PIN J5

#define GUEST_SCORE_BUTTON_PIN J6
#define GUEST_EXTRA_BUTTON_PIN J7
#define TIME_BUTTON_PIN J8

#define MINUS_BUTTON_PIN J12
#define PLUS_BUTTON_PIN J11
#define UNDO_BUTTON_PIN J9

// Beeper

#define BUZZER_PIN J10

#endif

#ifndef Buzzer_h
#define Buzzer_h

#include <Arduino.h>
#include "constants.h"

#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960

class Beeper {
    private:
        struct Tone {
            int frequency;
            unsigned long duration;
        };
        Tone READY[4] = {{NOTE_C6, 100}, {NOTE_E6, 100}, {NOTE_G6, 100}, {0, 0}};
        Tone CLICK[2] = {{NOTE_C7, 25}, {0, 0}};
        Tone TIME_START[2] = {{NOTE_C7, 100}, {0, 0}};
        Tone TIME_STOP[4] = {{NOTE_C7, 50}, {0, 25}, {NOTE_C7, 50}, {0, 0}};
        Tone LAST_TWO_MINUTES[2] = {{NOTE_C7, 250}, {0, 0}};
        Tone CONFIRMATION[4] = {{NOTE_B6, 50}, {0, 25}, {NOTE_D7, 50}, {0, 0}};
        Tone NOT_ALLOWED[2] = {{NOTE_C6, 250}, {0, 0}};
        uint8_t pin;
        unsigned long time;
        Tone *sequence = nullptr;
        uint8_t step;

    public:
        Beeper(
            uint8_t pin
        );
        void setup();
        void ready();
        void click();
        void timeStart();
        void timeStop();
        void lastTwoMinutes();
        void confirm();
        void notAllowed();
        void play(Tone *sequence);
        void loop();
};

#endif

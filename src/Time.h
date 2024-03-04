#ifndef Time_h
#define Time_h

#include <Arduino.h>

typedef union {
    struct {
        uint8_t min       : 6;
        uint8_t sec       : 6;
        uint8_t tenth     : 4;
    } fields;
    uint16_t time;
} Time;

#endif

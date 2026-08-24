#ifndef COMMON_INPUT_H
#define COMMON_INPUT_H

#include <stdbool.h>

typedef enum {
    INPUT_UP = 0,
    INPUT_DOWN,
    INPUT_START,
    INPUT_BUTTON_COUNT
} input_button_t;

typedef struct {
    bool held[INPUT_BUTTON_COUNT];
    bool pressed[INPUT_BUTTON_COUNT];
    bool released[INPUT_BUTTON_COUNT];
} input_state_t;

#endif /* COMMON_INPUT_H */

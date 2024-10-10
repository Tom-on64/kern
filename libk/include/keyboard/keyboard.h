#ifndef KEYBOARD_KEYBOARD_H
#define KEYBOARD_KEYBOARD_H

#include <stdbool.h>

typedef struct {
    char character;
    bool shift;
} keyboardState_t;

void setupKeyboard();

#endif

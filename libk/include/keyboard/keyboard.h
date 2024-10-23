#ifndef KEYBOARD_KEYBOARD_H
#define KEYBOARD_KEYBOARD_H

#include <interrupt/isr.h>
#include <stdbool.h>

typedef struct {
    char character;
    bool shift;
} keyboardState_t;

void keyboardHandler(intFrame_t*);

#endif

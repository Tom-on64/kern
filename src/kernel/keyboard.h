#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "stdint.h"

// Keycodes
#define LEFT_SHIFT_PRESS 0x2a
#define LEFT_SHIFT_RELEASE (LEFT_SHIFT_PRESS | 0x80)
#define RIGHT_SHIFT_PRESS 0x36
#define RIGHT_SHIFT_RELEASE (RIGHT_SHIFT_PRESS | 0x80)
#define LEFT_CTRL_PRESS 0x1D
#define LEFT_CTRL_RELEASE (LEFT_CTRL_PRESS | 0x80)
#define RIGHT_CTRL_PRESS 0x1D
#define RIGHT_CTRL_RELEASE (RIGHT_CTRL_PRESS | 0x80)
#define LEFT_ALT_PRESS 0x38
#define LEFT_ALT_RELEASE (LEFT_ALT_PRESS | 0x80)
#define RIGHT_ALT_PRESS 0x38
#define RIGHT_ALT_RELEASE (RIGHT_ALT_PRESS | 0x80)

#define MAX_BUFFER_SIZE 256

void setupKeyboard();
char* read(char terminator, uint8_t attr);

#endif

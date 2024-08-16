#ifndef KEYBOARD_KEYS_H
#define KEYBOARD_KEYS_H

#include <stdbool.h>

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

// Basic US Keyboard layout
static const char US_Keyboard[2][128] = {
    {
          0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 
        '\t','q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 
          0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'','`', 
          0, '\\','z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 
        '*',   0, ' ',  0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', // Numpad
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S',
        'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
        'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};

#endif

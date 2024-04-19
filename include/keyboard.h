#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "irq.h"
#include "idt.h"
#include "system.h"
#include "screen.h"
#include "stdint.h"
#include "stdbool.h"

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

const char US_Keyboard[2][128] = {
    {
        0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 
        0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 
        '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
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

uint8_t shift = 0;

uint16_t bufferLen = 0;
char buffer[MAX_BUFFER_SIZE + 1] = { 0 };
bool echo = false;
bool canBackspace = false;

void keyboardHandler() {
    uint8_t scancode;
    scancode = inb(0x60);

    if (scancode == LEFT_SHIFT_PRESS || scancode == RIGHT_SHIFT_PRESS) shift = 1;
    else if (scancode == LEFT_SHIFT_RELEASE || scancode == RIGHT_SHIFT_RELEASE) shift = 0;

    char c = US_Keyboard[shift][scancode];

    if (scancode & 0x80) { // Keyup
    } else if (c != 0) {
        if (!canBackspace && c == '\b') return;
        if (echo) putc(c);

        buffer[bufferLen] = c;

        if (c == '\b') bufferLen--;
        else bufferLen++;

        if (bufferLen > 0) canBackspace = true;
        else canBackspace = false;
    }
}

char* read(char terminator) {
    bufferLen = 0;
    echo = true;

    while (buffer[bufferLen-1] != terminator && bufferLen < MAX_BUFFER_SIZE);

    echo = false;
    canBackspace = false;
    buffer[bufferLen] = '\0';

    return buffer;
}

void setupKeyboard() {
    installIrqHandler(1, keyboardHandler);
}

#endif

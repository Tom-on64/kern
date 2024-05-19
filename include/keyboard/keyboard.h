#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <interrupt/idt.h>
#include <interrupt/pic.h>
#include <screen/text.h>
#include <keyboard/keys.h>
#include <stdbool.h>

#define MAX_INPUT_LENGTH 256

typedef struct {
    char character;
    uint8_t shift;
} keyboardState_t;

// TODO: Put at set address
keyboardState_t keyboard;

__attribute__ ((interrupt))
void keyboardHandler(intFrame_t* iframe) {
    uint8_t scancode = inb(0x60);
    char c = US_Keyboard[keyboard.shift][scancode];

    if (scancode == LEFT_SHIFT_PRESS || scancode == RIGHT_SHIFT_PRESS) {
        keyboard.shift = 1;
    } else if (scancode == LEFT_SHIFT_RELEASE || scancode == RIGHT_SHIFT_RELEASE) {
        keyboard.shift = 0;
    }

    if (scancode & 0x80) { // Keyup
    } else if (c != '\0') {
        keyboard.character = c;
    }

    sendPicEOI(1);
}

char getc() {
    keyboard.character = '\0';
    while (keyboard.character == '\0') __asm__ volatile ("hlt");
    return keyboard.character;
}

// Reads until a newline
char inputBuffer[MAX_INPUT_LENGTH];
char* read() {
    char c;
    uint8_t len = 0;

    while ((c = getc()) != '\n') {
        if (c == '\b') {
            if (len < 1) continue; // Can we backspace?
            len--;
            putc(c);
        } else if (len + 1 != MAX_INPUT_LENGTH) { // Check if we can add a new character
            inputBuffer[len++] = c;
            putc(c);
        }
    }

    inputBuffer[len++] = c; // Add newline
    putc(c);
    inputBuffer[len] = '\0'; // Null terminate
    return inputBuffer;
}

void setupKeyboard() {
    idtSetGate(0x21, keyboardHandler, INT_GATE_FLAGS);
    unsetIrqMask(1);
}

#endif

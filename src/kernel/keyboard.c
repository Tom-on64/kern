#include "keyboard.h"
#include "irq.h"
#include "idt.h"
#include "system.h"
#include "screen.h"
#include "stdint.h"

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

void keyboardHandler() {
    uint8_t scancode;
    scancode = inb(0x60);

    if (scancode == LEFT_SHIFT_PRESS || scancode == RIGHT_SHIFT_PRESS) shift = 1;
    else if (scancode == LEFT_SHIFT_RELEASE || scancode == RIGHT_SHIFT_RELEASE) shift = 0;

    char c = US_Keyboard[shift][scancode];

    if (scancode & 0x80) { // Keyup
    } else if (c != 0) {
        putc(c, 0x07);
    }
}

char* read(char terminator, uint8_t attr) {
    // TODO: This
}

void setupKeyboard() {
    installIrqHandler(1, keyboardHandler);
}


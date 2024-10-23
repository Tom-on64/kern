#include <keyboard/keyboard.h>
#include <interrupt/isr.h>
#include <keyboard/keys.h>
#include <memory/addresses.h>
#include <ports/io.h>
#include <stdio.h>
#include <stdbool.h>

static const uint8_t US_Keyboard[2][128] = US_KEYBOARD;
keyboardState_t* keyboard = (keyboardState_t*)KEYBOARD_STATE_LOC;

void keyboardHandler(intFrame_t*) { // TODO: Make this better and be able to read multi-byte scancode
    uint8_t scancode = inb(0x60);
    char c = US_Keyboard[keyboard->shift][scancode];

    if (scancode == LEFT_SHIFT_PRESS || scancode == RIGHT_SHIFT_PRESS) {
        keyboard->shift = true;
    } else if (scancode == LEFT_SHIFT_RELEASE || scancode == RIGHT_SHIFT_RELEASE) {
        keyboard->shift = false;
    }

    if (scancode & 0x80) { // Keyup
    } else if (c != '\0') {
        keyboard->character = c;
    }
}


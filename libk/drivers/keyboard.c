#include <drivers/keyboard.h>
#include <memory/addresses.h>
#include <cpu/system.h>
#include <cpu/isr.h>

static const uint8_t US_Keyboard[2][128] = US_KEYBOARD;
keyboardState_t* keyboard = (keyboardState_t*)KEYBOARD_STATE_LOC;

// TODO: Make this better and be able to read multi-byte scancode
void keyboardHandler(intFrame_t*) { 
    uint8_t scancode = inb(0x60);
    char c = US_Keyboard[keyboard->shift][scancode];

    if (scancode == LEFT_SHIFT_PRESS || scancode == RIGHT_SHIFT_PRESS) {
        keyboard->shift = 1;
    } else if (scancode == LEFT_SHIFT_RELEASE || scancode == RIGHT_SHIFT_RELEASE) {
        keyboard->shift = 0;
    }

    if (scancode & 0x80) { // Keyup
    } else if (c != '\0') { keyboard->character = c; }
}


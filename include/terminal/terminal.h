#ifndef TERMINAL_H
#define TERMINAL_H

#include <screen/gfxmode.h>
#include <memory/addresses.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t x;         // X Position of cursor in characters
    uint32_t y;         // Y Position of cursor in characters
    uint32_t bg;        // Printed character background
    uint32_t fg;        // Printed character foreground
    uint32_t width;     // Width in characters 
    uint32_t height;    // Height in characters
    char cursorChar;    // Character to use as a cursor
    bool showCursor;    // Should we display the cursor
} terminal_t;

terminal_t* terminal = (terminal_t*)TERMINAL_INFO_LOC;

#include <screen/text.h> // This needs to be here because it uses 'terminal' and otherwise it would be undeclared

int32_t terminalWrite(const void* buf, uint32_t len) {
    terminal->width = gfxMode->xRes / *(uint8_t*)FONT_WIDTH;
    terminal->height = gfxMode->yRes / *(uint8_t*)FONT_HEIGHT;

    char* str = (char*)buf;
    uint32_t i;

    // Do we need to check for NULL? Can we just skip it/ignore it?
    for (i = 0; i < len && str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            putcAt(' ', terminal->x, terminal->y);
            terminal->y++;
            terminal->x = 0;
        } else if (str[i] == '\b') {
            putcAt(' ', terminal->x, terminal->y);
            if (terminal->x == 0) {
                terminal->x = terminal->width - 1;
                terminal->y--; // Y could be 0?
            } else {
                terminal->x--;
            }
            putcAt(' ', terminal->x, terminal->y);
        } else if (str[i] == '\x1b') { // Escape sequence
            if (str[++i] != '[') { // TODO: Support more types of escape sequences
                continue;
            }
        } else {
            putcAt(str[i], terminal->x, terminal->y);
            terminal->x++;

            if (terminal->x >= terminal->width) { // Go to next line
                terminal->y++;
                terminal->x = 0;
            }
        }
    }
    scroll();

    return i; // Number of characters that were printed
} 

#endif

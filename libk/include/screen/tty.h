#ifndef _SCREEN_TTY_H
#define _SCREEN_TTY_H

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
    bool hideCursor;    // Should we display the cursor
} console_t;

extern console_t* console;

int32_t consoleWrite(const void* buf, uint32_t len);
void toggleCursor();

#endif

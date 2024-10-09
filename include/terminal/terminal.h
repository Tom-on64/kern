#ifndef TERMINAL_TERMINAL_H
#define TERMINAL_TERMINAL_H

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
} terminal_t;

extern terminal_t* terminal;

int32_t terminalWrite(const void* buf, uint32_t len);
void toggleCursor();

#endif

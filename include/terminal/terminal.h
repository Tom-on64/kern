#ifndef TERMINAL_TERMINAL_H
#define TERMINAL_TERMINAL_H

#include <screen/gfxmode.h>
#include <memory/addresses.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <color.h>

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

terminal_t* terminal = (terminal_t*)TERMINAL_INFO_LOC;

#include <screen/text.h> // This needs to be here because it uses 'terminal' and otherwise it would be undeclared

int32_t terminalWrite(const void* buf, uint32_t len) {
    terminal->width = gfxMode->xRes / *(uint8_t*)(FONT_LOC);
    terminal->height = gfxMode->yRes / *(uint8_t*)(FONT_LOC+1);

    char* str = (char*)buf;
    uint32_t i;

    // Do we need to check for NULL? Can we just skip it/ignore it?
    for (i = 0; i < len && str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            drawChar(' ', terminal->x, terminal->y, terminal->fg, terminal->bg);
            terminal->y++;
            terminal->x = 0;
        } else if (str[i] == '\b') {
            drawChar(' ', terminal->x, terminal->y, terminal->fg, terminal->bg);
            if (terminal->x == 0) {
                terminal->x = terminal->width - 1;
                terminal->y--; // Y could be 0?
            } else {
                terminal->x--;
            }
            drawChar(' ', terminal->x, terminal->y, terminal->fg, terminal->bg);
        } else if (str[i] == '\x1b') { // Escape sequence
            if (str[++i] != '[') { // TODO: Support more types of escape sequences
                i--;
                continue;
            }

            i++;
            uint32_t args[8] = { 0 };
            uint8_t argc = 0;
            while (1) { // I'm not 100% sure this works, but i'd like to imagine it does
                if (argc > 8) break; // Max arg count? idk

                uint32_t value = atoi(&str[i]);
                args[argc++] = value;

                while (value != 0) { // Skip by the number string
                    value /= 10;
                    i++;
                }

                if (str[i] != ';') break;
                i++;
            }

            switch (str[i]) { // TODO: Add default values (see docs/escape.txt)
                case 'A': // Cursor up
                    terminal->y -= args[0];
                    break;
                case 'B': // Cursor down
                    terminal->y += args[0];
                    break;
                case 'C': // Cursor left
                    terminal->x -= args[0];
                    break;
                case 'D': // Cursor right
                    terminal->x += args[0];
                    break;
                case 'E': // nth row
                    terminal->x = args[0];
                    break;
                case 'F': // nth col
                    terminal->y = args[0];
                    break;
                case 'G': // show/hide cursor
                    terminal->hideCursor = args[0] ? false : true;
                    break;
                case 'H': // move cursor to (x,y)
                    terminal->x = args[0];
                    terminal->y = args[1];
                    break;
                case 'I': // RESERVED
                case 'J': // Erase screen
                    clearScreen(terminal->bg);
                    break;
                case 'K': // Erase line
                    break; // TODO
                case 'L': // Erase collumn
                    break; // TODO
                case 'M': // Set FG
                    if (args[0] < 8) {
                        terminal->fg = codeToColor(args[0]);
                    } else if (args[0] == 8) {
                        terminal->fg = FG_COLOR;
                    }
                    break;
                case 'N': // Set BG
                    if (args[0] < 8) {
                        terminal->bg = codeToColor(args[0]);
                    } else if (args[0] == 8) {
                        terminal->bg = BG_COLOR;
                    }
                    break;
                default: break;
            }
        } else {
            drawChar(str[i], terminal->x, terminal->y, terminal->fg, terminal->bg);
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

void toggleCursor() {
    static bool visible = false;
    visible = !visible;

    drawChar(visible ? 127 : ' ', terminal->x, terminal->y, terminal->fg, terminal->bg);
}

#endif

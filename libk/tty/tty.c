#include <tty/tty.h>
#include <screen/gfxmode.h>
#include <screen/text.h>
#include <memory/addresses.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/gfx.h>

console_t* console = (console_t*)TERMINAL_INFO_LOC;

int32_t consoleWrite(const void* buf, uint32_t len) { // TODO fix font size
    console->width = gfxMode->xRes / 8;//*(uint8_t*)(FONT_LOC);
    console->height = gfxMode->yRes / 16;//*(uint8_t*)(FONT_LOC+1);

    char* str = (char*)buf;
    uint32_t i;

    // Do we need to check for NULL? Can we just skip it/ignore it?
    for (i = 0; i < len && str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            drawChar(' ', console->x, console->y, console->fg, console->bg);
            console->y++;
            console->x = 0;
        } else if (str[i] == '\b') {
            drawChar(' ', console->x, console->y, console->fg, console->bg);
            if (console->x == 0) {
                console->x = console->width - 1;
                console->y--; // Y could be 0?
            } else {
                console->x--;
            }
            drawChar(' ', console->x, console->y, console->fg, console->bg);
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
                    console->y -= args[0];
                    break;
                case 'B': // Cursor down
                    console->y += args[0];
                    break;
                case 'C': // Cursor left
                    console->x -= args[0];
                    break;
                case 'D': // Cursor right
                    console->x += args[0];
                    break;
                case 'E': // nth row
                    console->x = args[0];
                    break;
                case 'F': // nth col
                    console->y = args[0];
                    break;
                case 'G': // show/hide cursor
                    console->hideCursor = args[0] ? false : true;
                    break;
                case 'H': // move cursor to (x,y)
                    console->x = args[0];
                    console->y = args[1];
                    break;
                case 'I': // RESERVED
                case 'J': // Erase screen
                    clearScreen(console->bg);
                    break;
                case 'K': // Erase line
                    break; // TODO
                case 'L': // Erase collumn
                    break; // TODO
                case 'M': // Set FG
                    if (args[0] < 8) {
                        console->fg = codeToColor(args[0]);
                    } else if (args[0] == 8) {
                        console->fg = FG_COLOR;
                    }
                    break;
                case 'N': // Set BG
                    if (args[0] < 8) {
                        console->bg = codeToColor(args[0]);
                    } else if (args[0] == 8) {
                        console->bg = BG_COLOR;
                    }
                    break;
                default: break;
            }
        } else {
            drawChar(str[i], console->x, console->y, console->fg, console->bg);
            console->x++;

            if (console->x >= console->width) { // Go to next line
                console->y++;
                console->x = 0;
            }
        }
    }
    scroll();

    return i; // Number of characters that were printed
}

void toggleCursor() {
    static bool visible = false;
    visible = !visible;

    drawChar(visible ? 127 : ' ', console->x, console->y, console->fg, console->bg);
}


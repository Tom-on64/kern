#include <stdint.h>
#include <color.h>

uint32_t codeToColor(uint8_t code) {
    switch (code) {
        case 0: return BLACK;
        case 1: return RED;
        case 2: return GREEN;
        case 3: return YELLOW;
        case 4: return BLUE;
        case 5: return MAGENTA;
        case 6: return CYAN;
        case 7: return WHITE;
        default: return BLACK;
    }
}


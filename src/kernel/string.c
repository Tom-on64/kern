#include "string.h"

// Source: http://www.strudel.org.uk/itoa/
char* itoa(uint32_t val, uint8_t base) {
    static char buf[32] = { 0 };

    if (base == 0 || base > 16) {
        // TODO: Add errors!!
        return buf;
    }
 
    int i = 30;
    for (; val && i; i--, val /= base) {
        buf[i] = "0123456789abcdef"[val % base];
    }

    return &buf[i + 1];
}


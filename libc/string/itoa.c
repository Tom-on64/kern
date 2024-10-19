#include <string.h>

// Source: http://www.strudel.org.uk/itoa/
char* itoa(size_t val, int base) {
    static char buf[32] = { 0 };
    
    if (base == 0 || base > 16) { return NULL; }

    if (val == 0) {
        buf[0] = '0';
        return buf;
    }
 
    int i;
    for (i = 30; val && i; i--, val /= base) {
        buf[i] = "0123456789abcdef"[val % base];
    }

    return &buf[i + 1];
}

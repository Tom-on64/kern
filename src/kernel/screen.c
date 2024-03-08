#include "screen.h"

unsigned int cursor = 0;

void putc(char c, char attr) {
    int mem = VIDMEM + cursor * 2;
    *(char*)(mem) = c;
    *(char*)(mem+1) = attr;

    cursor++;
}

void print(char* s, char attr) {
    int i = 0;
    char c = *s;
    while (c != '\0') {
        putc(c, attr);
        c = *(s + ++i);
    }
}


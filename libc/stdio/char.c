#include <syscall.h>
#include <stdio.h>

void putc(char c) {
    // write(stdout, &c, 1); TODO: This doesn't work and we need to use a static variable
    static char _c;
    _c = c;
    write(1, &_c, 1);
}

char getc() {
    char* c = (char*)0x2100; // TODO: Don't use a random looking memory address -_('-')_-
    *c= '\0';
    while (*c == '\0') { __asm__ volatile ("hlt"); }
    return *c;
}


#ifndef STDIO_H
#define STDIO_H

#include <syscall.h>
#include <string.h>

// TODO: Make a proper file descriptor table
#define STDOUT 1

void print(const char* s) {
    write(STDOUT, s, strlen(s));
}

void putc(char c) {
    static char _c; // We need to pass in a static address. Idk why
    _c = c;
    write(STDOUT, &_c, 1);
}

char getc() {
    char* character = (char*)0x2100;
    *character = '\0';
    while (*character == '\0') { __asm__ volatile ("hlt"); }
    return *character;
}

// Reads until a newline
// TODO: Use Read() Syscall
void read(char* buffer) {
    char c;
    uint8_t len = 0;

    while ((c = getc()) != '\n') {
        if (c == '\b') {
            if (len < 1) continue; // Can we backspace?
            len--;
            putc(c);
        } else {
            buffer[len++] = c;
            putc(c);
        }
    }

    buffer[len++] = c; // Add newline
    putc(c);
    buffer[len] = '\0'; // Null terminate
}

#endif

#ifndef STDIO_H
#define STDIO_H

#include <syscall.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

/* Declarations */
void putc(char);
char getc();

int printf(const char*, ...);

/* STDIO Include */
#include <_fileio.h>
#include <_printf.h>

void putc(char c) {
    // write(stdout, &c, 1); TODO: This doesn't work and we need to use a static variable
    static char _c;
    _c = c;
    write(1, &_c, 1);
}

char getc() {
    char* c = (char*)0x2100;
    *c= '\0';
    while (*c == '\0') { __asm__ volatile ("hlt"); }
    return *c;
}

void print(const char* s) {
    write(1, s, strlen(s));
}

void clear() {
    print("\x1b[J");
}

// Reads until a newline
// TODO: Use Read() Syscall
void reads(char* buffer) {
    char c;
    int len = 0;

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

    putc(c); // Print the newline
    buffer[len] = '\0'; // Null terminate
}

#endif

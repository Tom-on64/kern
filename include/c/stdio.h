#ifndef STDIO_H
#define STDIO_H

#include <syscall.h>
#include <string.h>

// TODO: Make a proper file descriptor table
#define STDOUT 1

/* --- Output --- */
void putc(char c) {
    // write(STDOUT, &c, 1); TODO: This doesn't work and we need to use a static variable
    static char _c;
    _c = c;
    write(STDOUT, &_c, 1);
}

void print(const char* s) {
    write(STDOUT, s, strlen(s));
}

void clear() {
    print("\x1b[J");
}

// TODO: Dynamically allocate memory and print once instead of calling putc for every character :/
// TODO: Support negative numbers
// TODO: Printing individual characters doesn't let escape sequences work :(
int printf(const char* fmt, ...) {
    char* argPtr = (char*)&fmt + sizeof(char*); // First arg after fmt
    uint8_t state = 0;
    
    for (uint32_t i = 0; fmt[i] != '\0'; i++) {
        char c = fmt[i];

        if (state == 0) {
             if (c == '%') state = '%';
             else putc(c);
        } else if (state == '%') {
            switch (c) {
                case 'd': // Decimal number
                    print(itoa(*(uint32_t*)argPtr, 10));
                    argPtr += sizeof(unsigned int);
                    break;
                case 'x': // Hex number
                    print(itoa(*(uint32_t*)argPtr, 16));
                    argPtr += sizeof(unsigned int);
                    break;
                case 's': // String
                    print(*(char**)argPtr);
                    argPtr += sizeof(char*);
                    break;
                case 'c': // Char
                    putc(*(char*)argPtr);
                    argPtr += sizeof(char);
                    break;
                case '%': // Just print '%'
                    putc('%');
                    break;
                default:
                    putc('%');
                    putc(c);
                    break;
            }
            state = 0;
        }
    }

    return 0;
}

/* --- Input --- */
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

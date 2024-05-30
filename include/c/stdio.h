#ifndef STDIO_H
#define STDIO_H

#include <syscall.h>
#include <string.h>
#include <stdlib.h>

// TODO: Make a proper file descriptor table
#define stdin  0
#define stdout 1
#define stderr 2

/* --- Output --- */
void putc(char c) {
    // write(stdout, &c, 1); TODO: This doesn't work and we need to use a static variable
    static char _c;
    _c = c;
    write(stdout, &_c, 1);
}

void print(const char* s) {
    write(stdout, s, strlen(s));
}

void clear() {
    print("\x1b[J");
}

// TODO: Support negative numbers
int printf(const char* fmt, ...) {
    uint32_t* argPtr = (uint32_t*)&fmt + 1; // First arg after fmt
    
    // Printing buffer
    static char* buffer;
    static uint32_t len;
    uint32_t max = 256;

    buffer = malloc(max);
    len = 0;
    
    uint8_t state = 0;
    for (uint32_t i = 0; fmt[i] != '\0'; i++) {
        if (len > max) {
            max *= 2;
            // Reallocate larger buffer TODO: Use realloc();
            char* temp = malloc(max);
            strcpy(temp, buffer);
            free(buffer);
            buffer = temp;
        }
        buffer[len] = '\0';

        char c = fmt[i];
        if (state == 0) {
             if (c == '%') state = '%';
             else buffer[len++] = c;
        } else if (state == '%') {
            switch (c) {
                case 'd': // Decimal number
                    strcat(buffer, itoa(*(uint32_t*)argPtr++, 10));
                    len += strlen(&buffer[len]); // Add the remaining length
                    break;
                case 'x': // Hex number
                    strcat(buffer, itoa(*(uint32_t*)argPtr++, 16));
                    len += strlen(&buffer[len]);
                    break;
                case 's': // String
                    char* s = *(char**)argPtr++;
                    
                    while (*s != '\0') {
                        buffer[len++] = *s++;
                    }
                    break;
                case 'c': // Char
                    buffer[len++] = *(char*)argPtr;
                    argPtr++;
                    break;
                default: // This looks insane
                    buffer[len++] = '%';
                case '%': // Just print '%'
                    buffer[len++] = c;
                    break;
            }
            state = 0;
        }
    }

    buffer[len] = '\0';
    write(stdout, buffer, len);
    free(buffer);

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

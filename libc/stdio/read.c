#include <stdio.h>

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


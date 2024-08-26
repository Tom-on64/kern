#ifndef _PRINTF_H
#define _PRINTF_H

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

// TODO: Support negative numbers
int printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    // Printing buffer
    static char* buffer;
    static unsigned int len;
    int max = 256;

    buffer = malloc(max);
    len = 0;
    
    int state = 0;
    for (int i = 0; fmt[i] != '\0'; i++) {
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
                    strcat(buffer, itoa(va_arg(args, int), 10));
                    len += strlen(&buffer[len]); // Add the remaining length
                    break;
                case 'x': // Hex number
                    strcat(buffer, itoa(va_arg(args, int), 16));
                    len += strlen(&buffer[len]);
                    break;
                case 's': // String
                    char* s = va_arg(args, char*);
                    
                    while (*s != '\0') {
                        buffer[len++] = *s++;
                    }
                    break;
                case 'c': // Char
                    buffer[len++] = va_arg(args, char);
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
    write(1, buffer, len);
    free(buffer);
    va_end(args);

    return 0;
}

#endif


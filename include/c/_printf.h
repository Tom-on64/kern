#ifndef _PRINTF_H
#define _PRINTF_H

#include <syscall.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <_fileio.h>

// Warning: returns a allocated buffer, must be freed!
char* _format(const char* fmt, va_list args) {
    static char* buffer;
    size_t len = 0;
    size_t max = 256;

    buffer = malloc(max);

    size_t i = 0;
    while (fmt[i] != '\0') {
        if (len > max) {
            max *= 2;
            // Reallocate a larger buffer
            char* temp = malloc(max);
            strcpy(temp, buffer);
            free(buffer);
            buffer = temp;
        }

        // Formatting magic...
        if (fmt[i] == '%') { i++;
            /* TODO:
             %p - pointer (0x########)
             %n - ""
             */
            int pad = -1;
            char padc = ' ';

            if (fmt[i] == '0') { i++; padc = '0'; }
            if (fmt[i] >= '0' && fmt[i] <= '9') {
                pad = atoi(fmt + i);
                while (fmt[i] >= '0' && fmt[i] <= '9') { i++; }
            }
            while (pad > 0) { buffer[len++] = padc; pad--; }

            char c = fmt[i++];
            if (c == 'd' || c == 'x' || c == 'o') {
                int base = 10;
                if (c == 'x') base = 16;
                else if (c == 'o') base = 8;
                
                char* s = itoa(va_arg(args, int), base);
                if (pad == 0) len -= strlen(s);
                while ((buffer[len++] = *s++) != '\0');
                len--;
            } else if (c == 'c') {
                if (pad == 0) len--;
                buffer[len++] = va_arg(args, char);
            } else if (c == 's') {
                char* s = va_arg(args, char*);
                if (pad == 0) len -= strlen(s);
                while ((buffer[len++] = *s++) != '\0');
                len--;
            } else if (c == '%') {
                buffer[len++] = '%';
            } else {
                buffer[len++] = '%';
                if (padc == '0') { buffer[len++] = '0'; }
                if (pad > 0) { buffer[len++] = '#'; } // TODO
                buffer[len++] = c;
            }
        } else { buffer[len++] = fmt[i++]; }
    }
    
    buffer[len] = '\0';
    return buffer;
}

int sprintf(char* str, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char* buffer = _format(fmt, args);
    strcpy(str, buffer);
    free(buffer);
    va_end(args);

    return strlen(str);
}

int snprintf(char* str, size_t n, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char* buffer = _format(fmt, args);
    strncpy(str, buffer, n);
    free(buffer);
    va_end(args);

    return strlen(str);
}

int fprintf(FILE* stream, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char* buffer = _format(fmt, args);
    size_t len = strlen(buffer);
    write(stream->_file, buffer, len); // TODO: use fwrite()
    
    free(buffer);
    va_end(args);
    return len;
}

int printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char* buffer = _format(fmt, args);
    size_t len = strlen(buffer);
    write(stdout->_file, buffer, len); // TODO: use fwrite()
    
    free(buffer);
    va_end(args);
    return len;
}

#endif


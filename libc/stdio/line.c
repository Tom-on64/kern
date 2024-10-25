#include <stdio.h>
#include <stddef.h>

char* fgets(char* str, int n, FILE* stream) {
    if (!str || !stream || n <= 1) { return NULL; }

    char* dest = str;
    int ch;

    while (--n > 0 && (ch = getc(stream)) != EOF) {
        *dest++ = ch;
        if (ch == '\n') break;  // Stop at newline
    }

    *dest = '\0';   // Null terminate

    return (dest == str) ? NULL : str;
}

int fputs(const char* str, FILE* stream) {
    if (!str || !stream) { return EOF; }

    while (*str) {
        if (putc(*str++, stream) == EOF) { return EOF; }
    }

    return 0;
}

inline char* gets(char* str, int n) { return fgets(str, n, stdin); }
inline int puts(const char* str) { return fputs(str, stdout); }


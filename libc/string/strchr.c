#include <string.h>

char* strchr(const char* str, char c) {
    const char* s = str;
    while (*s != '\0' && *s != c) { s++; }
    return (*s == c) ? (char*)s : NULL;
}

char* strrchr(const char* str, char c) { return NULL; } // TODO


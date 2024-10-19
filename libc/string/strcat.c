#include <string.h>

char* strcat(char* str1, const char* str2) {
    char* s = str1;
    while (*s != '\0') { s++; }
    strcpy(s, str2);
    return str1;
}

char* strncat(char* str1, const char* str2, size_t n) {
    char* s = str1;
    while (*s != '\0') { s++; }
    strncpy(s, str2, n);
    return str1;
}

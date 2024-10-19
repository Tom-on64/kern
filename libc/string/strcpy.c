#include <string.h>

char* strcpy(char* str1, const char* str2) {
    char* dst = str1;
    const char* src = str2;
    while ((*dst++ = *src++) != '\0');
    return str1;
}

char* strncpy(char* str1, const char* str2, size_t n) {
    char* dst = str1;
    const char* src = str2;
    while (n > 0) {
        n--;
        if ((*dst++ = *src++) == '\0') {
            memset(dst, '\0', n);
            break;
        }
    }
    return str1;
}


#include <string.h>

int strcmp(const char* str1, const char* str2) {
    while (*str1 != '\0' && *str1 == *str2) {
        str1++;
        str2++;
    }

    uint8_t c1 = (*(uint8_t*)str1);
    uint8_t c2 = (*(uint8_t*)str2);

    return ((c1 < c2) ? -1 : (c1 > c2));
}

int strncmp(const char* str1, const char* str2, size_t n) {
    if (n == 0) { return 0; }
    while (n-- > 0 && *str1 == *str2) {
        if (n == 0 || *str1 == '\0') { return 0; }
        str1++;
        str2++;
    }

    uint8_t c1 = (*(uint8_t*)str1);
    uint8_t c2 = (*(uint8_t*)str2);

    return ((c1 < c2) ? -1 : (c1 > c2));
}

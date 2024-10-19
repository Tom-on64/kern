#include <string.h>
#include <stddef.h>

size_t strlen(const char* str) {
    const char* p = str;
    while (*++p != '\0');
    return (size_t)(p - str);
}

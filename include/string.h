#ifndef STRING_H
#define STRING_H

#include "stdint.h"

// Source: http://www.strudel.org.uk/itoa/
char* itoa(uint32_t val, uint8_t base) {
    static char buf[32] = { 0 };

    if (base == 0 || base > 16) {
        // TODO: Add errors!!
        return buf;
    }
 
    int i = 30;
    for (; val && i; i--, val /= base) {
        buf[i] = "0123456789abcdef"[val % base];
    }

    return &buf[i + 1];
}

uint32_t atoi(const char* str) {
    uint32_t res = 0;

    // TODO: Add error checking

    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str++ - '0');
    }

    return res;
}

int isSpace(char c) {
    if (c == ' ' || c == '\t' || c == '\n') return 1;
    else return 0;
}

// Some implementations taken from https://en.wikibooks.org/wiki/C_Programming/String_manipulation
char* strcpy(char* str1, const char* str2) {
    char* dst = str1;
    const char* src = str2;
    while ((*dst++ = *src++) != '\0');
    return str1;
}

char* strcat(char* str1, const char* str2) {
    char* s = str1;
    while (*s != '\0') { s++; }
    strcpy(s, str2);
    return str1;
}

char* strchr(const char* str, char c) {
    const char* s = str;
    while (*s != '\0' && *s != c) { s++; }
    return (*s == c) ? (char*)s : NULL;
}

int strcmp(const char* str1, const char* str2) {
    while (*str1 != '\0' && *str1 == *str2) {
        str1++;
        str2++;
    }

    uint8_t c1 = (*(uint8_t*)str1);
    uint8_t c2 = (*(uint8_t*)str2);

    return ((c1 < c2) ? -1 : (c1 > c2));
}

uint32_t strlen(const char* str) {
    const char* p = str;
    while (*++p != '\0');
    return (uint32_t)(p - str);
}

// Memory functions
char* memcopy(char* src, char* dst, uint16_t length) {
    for (int i = 0; i < length; i++) {
        *(dst + i) = *(src + i);
    }
    return dst;
}

char* memset(char* dst, char value, uint16_t length) {
    for (int i = 0; i < length; i++) {
        *(dst + i) = value;
    }
    return dst;
}

#endif

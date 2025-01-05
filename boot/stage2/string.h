#ifndef _STRING_H
#define _STRING_H

#include "stdint.h"

char* itoa(size_t val, int base);
void* memcpy(void* src, void* dst, size_t length);
void* memset(void* dst, uint8_t value, size_t length);
char* strcpy(char* str1, const char* str2);
char* strcat(char* str1, const char* str2);
char* strchr(const char* str, char c);
int strcmp(const char* str1, const char* str2);
size_t strlen(const char* str);

#endif

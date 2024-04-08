#ifndef STRING_H
#define STRING_H

#include "stdint.h"

char* itoa(uint32_t val, uint8_t base);

char* strcat(char* str1, const char* str2);
char* strchr(const char* str, char c);
int strcmp(const char* str1, const char* str2);
char* strcpy(char* str1, const char* str2);
uint32_t strlen(const char* str);

#endif

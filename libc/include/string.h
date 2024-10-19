#ifndef _STRING_H
#define _STRING_H

#include <stdint.h>
#include <stddef.h>

void* memcpy(void* src, void* dst, size_t length);
void* memcpy32(void* src, void* dst, size_t length);
void* memset(void* dst, uint8_t value, size_t length);
void* memset32(void* dst, uint32_t value, size_t length);

char* itoa(size_t val, int base);
uint32_t atoi(const char* str);

char* strcpy(char* str1, const char* str2);
char* strncpy(char* str1, const char* str2, size_t n);

char* strcat(char* str1, const char* str2);
char* strncat(char* str1, const char* str2, size_t n);

char* strchr(const char* str, char c);
char* strrchr(const char* str, char c);

int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t n);

size_t strlen(const char* str);

#endif

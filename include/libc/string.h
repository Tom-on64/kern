#ifndef STRING_H
#define STRING_H

#include <stdint.h>

void* memcpy(void* src, void* dst, uint32_t length);
void* memcpy32(void* src, void* dst, uint32_t length);
void* memset(void* dst, uint8_t value, uint32_t length);

char* itoa(uint32_t val, uint8_t base);
uint32_t atoi(const char* str);
int isSpace(char c);
char* strcpy(char* str1, const char* str2);
char* strncpy(char* str1, const char* str2, size_t n);
char* strcat(char* str1, const char* str2);
char* strncat(char* str1, const char* str2, size_t n);
char* strchr(const char* str, char c);
char* strrchr(const char* str, char c);
int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t n);
uint32_t strlen(const char* str);

#endif

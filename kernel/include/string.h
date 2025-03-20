#ifndef _STRING_H
#define _STRING_H

#include <kernel.h>

// Memory functions
void*	memchr(const void* ptr, uint8_t val, size_t len);
int	memcmp(const void* ptr1, const void* ptr2, size_t len);
void*	memcpy(void* dst, const void* src, size_t len);
void*	memmov(void* dst, const void* src, size_t len);
void*	memset(void* dst, uint8_t val, size_t len);

// String functions
char*	strbrk(char* str, const char* accept);
char*	strcat(char* dst, const char* src);
char*	strchr(const char* str, char c);
int	strcmp(const char* str1, const char* str2);
char*	strcpy(char* dst, const char* src);
size_t	strlen(const char* str);
size_t	strspn(const char* str, const char* accept);
char*	strstr(const char* str1, const char* str2);
char*	strtok(char* str, const char* delim);

#endif

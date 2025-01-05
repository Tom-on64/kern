#include "stdint.h"
#include "string.h"

char* itoa(size_t val, int base) {
	static char buf[32] = { 0 };

	if (base == 0 || base > 16) return NULL; 

	if (val == 0) {
		buf[0] = '0';
		buf[1] = '\0';
		return buf;
	}

	int i;
	buf[31] = '\0';
	for (i = 30; val && i; i--, val /= base) {
		buf[i] = "0123456789abcdef"[val % base];
	}

	return &buf[i + 1];
}

void* memcpy(void* src, void* dst, size_t length) {
	uint8_t* p = dst;
	size_t i = 0;
	while (i < length) p[i] = *(uint8_t*)(src + i);
	return dst;
}

void* memset(void* dst, uint8_t val, size_t length) {
	uint8_t* p = dst;
	size_t i = 0;
	while (i < length) p[i++] = val;
	return dst;
}

char* strcpy(char* str1, const char* str2) {
	char* p = str1;
	while (*str2 != '\0') *p++ = *str2++;
	return str1;
}

char* strcat(char* str1, const char* str2) {
	char* p = str1;
	while (*str2++ != '\0');
	while (*str2 != '\0') *p++ = *str2++;
	return str1;
}

char* strchr(const char* str, char c) {
	char* p = (char*)str;
	while (*p != '\0' && *p != c) p++;
    	return (*p == c) ? p : NULL;
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

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len++] != '\0');
	return len;
}


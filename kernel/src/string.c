#include <kernel.h>
#include <string.h>

void* memchr(const void* ptr, uint8_t val, size_t len) {
	const uint8_t* p = ptr;
	while (len--) if (*p++ == val) return (void*)(p - 1);
	return NULL;
}

int memcmp(const void* ptr1, const void* ptr2, size_t len) {
	const uint8_t* p1 = ptr1;
	const uint8_t* p2 = ptr2;
	while (len--) if (*p1++ != *p2++) return p1[-1] - p2[-1];
	return 0;
}

void* memcpy(void* dst, const void* src, size_t len) {
	uint8_t* d = dst;
	const uint8_t* s = src;
	while (len--) *d++ = *s++;
	return dst;
}

void* memmov(void* dst, const void* src, size_t len) {
	uint8_t* d = dst;
	const uint8_t* s = src;
	if (d < s) while (len--) *d++ = *s++;
	else while (len--) d[len] = s[len];
	return dst;
}

void* memset(void* dst, uint8_t val, size_t len) {
	uint8_t* d = dst;
	while (len--) *d++ = val;
	return dst;
}

char* strbrk(char* str, const char* accept) {
	while (*str) {
		if (strchr(accept, *str)) return (char*)str;
		str++;
	}
	return NULL;
}

char* strcat(char* dst, const char* src) {
	char* d = dst;
	while (*d) d++;
	while ((*d++ = *src++));
	return dst;
}

char* strchr(const char* str, char c) {
	while (*str) if (*str++ == c) return (char*)(str - 1);
	return NULL;
}

int strcmp(const char* str1, const char* str2) {
	while (*str1 && (*str1 == *str2)) str1++, str2++;
	return (int)(*(uint8_t*)str1 - *(uint8_t*)str2);
}

char* strcpy(char* dst, const char* src) {
	char* d = dst;
	while ((*d++ = *src++));
	return dst;
}

size_t strlen(const char* str) {
	const char* s = str;
	while (*s++);
	return s - str;
}

size_t strspn(const char* str, const char* accept) {
	const char* s = str;
	while (*s && strchr(accept, *s)) s++;
	return s - str;
}

char* strstr(const char* str1, const char* str2) {
	if (!*str2) return (char*)str1;
	for (; *str1; str1++) {
		const char* s1 = str1;
		const char* s2 = str2;
		while (*s1 && *s2 && (*s1 == *s2)) s1++, s2++;
		if (!*s2) return (char*)str1;
	}
	return NULL;
}

char* strtok(char* str, const char* delim) {
	static char* next;
	if (!str) str = next;
	if (!str) return NULL;
	str += strspn(str, delim);
	if (!*str) return (next = NULL);
	char* tok = str;
	str = strbrk(str, delim);
	if (str) *str++ = '\0';
	next = str;
	return tok;
}


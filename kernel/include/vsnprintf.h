#ifndef _VSNPRINTF_H
#define _VSNPRINTF_H

#include <kernel.h>

int snprintf(char *buf, size_t n, const char *fmt, ...) __printf(3, 4);
int vsnprintf(char* buf, size_t n, const char* fmt, va_list ap);

#endif

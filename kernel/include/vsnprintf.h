#ifndef _VSNPRINTF_H
#define _VSNPRINTF_H

#include <kernel.h>

int snprintf(char *buf, size_t n, char *fmt, ...);
int vsnprintf(char* buf, size_t n, char* fmt, va_list ap);

#endif

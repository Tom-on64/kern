#ifndef _VSNPRINTF_H
#define _VSNPRINTF_H

#include <kernel.h>

int vsnprintf(char* buf, size_t len, char* fmt, va_list args);

#endif

#ifndef _STDIO_H
#define _STDIO_H

#include "stdarg.h"
#include "stdint.h"

#define VIDMEM	0xb8000
#define COLS	80
#define ROWS	25

void setAttr(uint8_t attr);
void setCursor(uint32_t row, uint32_t col);
void updateCursor();
void hideCursor();
void showCursor();

void cls();
void newline();
void putc(int c);
void puts(char* s);
int printf(char* fmt, ...);
int vsnprintf(void* buf, size_t len, char* fmt, va_list args);
void putat(uint32_t row, uint32_t col, int c);

#endif

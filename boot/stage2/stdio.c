#include "stdarg.h"
#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "port.h"

static uint32_t xpos;
static uint32_t ypos;
static uint8_t attr;

void setAttr(uint8_t newattr) { attr = newattr; }

void setCursor(uint32_t row, uint32_t col) {
	xpos = col;
	ypos = row;
	updateCursor();
}

void updateCursor() {
	uint16_t pos = xpos + ypos * COLS;

	outb(0x3d4, 0x0f);
	outb(0x3d5, pos & 0xff);
	outb(0x3d4, 0x0e);
	outb(0x3d5, (pos >> 8) & 0xff);
}

void hideCursor() {
	outb(0x3d4, 0x0a);
	outb(0x3d5, 0x20);
}

void showCursor() {
	uint8_t start 	= 0;
	uint8_t end 	= 15;

	outb(0x3d4, 0x0a);
	outb(0x3d5, (inb(0x3d5) & 0xc0) | start);
	outb(0x3d4, 0x0b);
	outb(0x3d5, (inb(0x3d5) & 0xe0) | end);
}

void cls() {
	uint8_t* vidmem = (uint8_t*)VIDMEM;

	for (size_t i = 0; i < ROWS * COLS; i++) {
		vidmem[i * 2] = 0;
		vidmem[i * 2 + 1] = attr;
	}
	
	xpos = 0;
	ypos = 0;
	updateCursor();
}

void putat(uint32_t row, uint32_t col, int c) {
	uint8_t* vidmem = (uint8_t*)VIDMEM;
	vidmem[(col + row * COLS) * 2] = c & 0xFF;
	vidmem[(col + row * COLS) * 2 + 1] = attr;
}

void newline() {
	xpos = 0;
	ypos++;
	if (ypos >= ROWS) ypos = 0;
	updateCursor();
}

void putc(int c) {
	putat(ypos, xpos, c);
	xpos++;
	if (xpos >= COLS) newline();
	updateCursor();
}

void puts(char* s) {
	while (*s != '\0') {
		if (*s == '\n' || *s == '\r') {
			newline();
			s++;
		} else putc(*s++);
	}
}

int vsnprintf(void* buf, size_t len, char* fmt, va_list args) {
	char* str = (char*)buf;
	size_t pos = 0;

	while (*fmt != '\0') {
		if (*fmt != '%') {
			if (pos < len - 1) str[pos++] = *fmt++;
			continue;
		}

		fmt++;
		char pad = ' ';
		int width = 0;

		if (*fmt == '0') pad = '0';
		while (*fmt >= '0' && *fmt <= '9') {
			width *= 10;
			width += *fmt++ - '0';
		}

		// DO NOT TALK ABOUT THIS ABOMINATION
		char* s;
		switch (*fmt++) {
		case 'd': s = itoa(va_arg(args, int), 10); goto string;
		case 'x': s = itoa(va_arg(args, int), 16); goto string;
		case 's': s = va_arg(args, char*); goto string;
		case 'c': if (pos < len - 1) str[pos++] = va_arg(args, char); break;
		case '%': if (pos < len - 1) str[pos++] = '%'; break;
		default:
			if (pos < len - 1) str[pos++] = '%';
			if (pos < len - 1) str[pos++] = *(fmt - 1);
			break;
		string: 
			width -= strlen(s) - 1;
			if (width < 0) width = 0;
			while (width-- && pos < len - 1) str[pos++] = pad;
			while (*s && pos < len - 1) str[pos++] = *s++;
			break;
		}
	}

	if (len > 0) {
		if (pos < len) str[pos++] = '\0';
		else str[len - 1] = '\0';	// Shouldn't be able to be reached, but who knows
	}

	return pos;
}

int printf(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char buf[256];
	int ret = vsnprintf(buf, 256, fmt, args);
	va_end(args);
	puts(buf);
	return ret;
}


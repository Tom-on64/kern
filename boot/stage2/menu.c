#include "stdarg.h"
#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "menu.h"

/*
 * The reason we subtract one from the width and height (or length) is because 
 * we use the (in this example) width to offset from the (in this example) x 
 * position. If something has a width of 1 character, the end of it should be 
 * at 'x + w', so if 'w' were 1, it would be off by one. Therefore we subtract 
 * one from the given dimensions.
 */

void rect(uint32_t x, uint32_t y, uint32_t _w, uint32_t _h) {
	if (_w < 2 || _h < 2) return;
	uint32_t w = _w - 1;
	uint32_t h = _h - 1;
	
	// Top
	putat(y, x, CORNER_TL);
	for (size_t dx = 1; dx < w; dx++) putat(y, x + dx, LINE_H);
	putat(y, x + w, CORNER_TR);

	// Sides
	for (size_t dy = 1; dy < h; dy++) {
		putat(y + dy, x, LINE_V);
		putat(y + dy, x + w, LINE_V);
	}

	// Bottom
	putat(y + h, x, CORNER_BL);
	for (size_t dx = 1; dx < w; dx++) putat(y + h, x + dx, LINE_H);
	putat(y + h, x + w, CORNER_BR);
}

void fill(uint32_t x, uint32_t y, uint32_t w, uint32_t h, char c) {
	// We don't add one to the dimensions here
	for (size_t dy = 0; dy < h; dy++) {
		for (size_t dx = 0; dx < w; dx++) {
			putat(y + dy, x + dx, c);
		}
	}
}

void hline(uint32_t x, uint32_t y, uint32_t _len) {
	uint32_t len = _len - 1;
	putat(y, x, LINE_T_RIGHT);
	for (size_t dx = 1; dx < len; dx++) putat(y, x + dx, LINE_H);
	putat(y, x + len, LINE_T_LEFT);
}

void vline(uint32_t x, uint32_t y, uint32_t _len) {
	uint32_t len = _len - 1;
	putat(y, x, LINE_T_DOWN);
	for (size_t dy = 1; dy < len; dy++) putat(y + dy, x, LINE_V);
	putat(y + len, x, LINE_T_UP);
}

void text(uint32_t x, uint32_t y, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char buf[256];
	vsnprintf(buf, 256, fmt, args);
	va_end(args);

	uint32_t len = strlen(buf);
	uint32_t basex = x - len / 2;

	size_t i = 0;
	while (buf[i] != '\0') putat(y, basex + i, buf[i++]);
}


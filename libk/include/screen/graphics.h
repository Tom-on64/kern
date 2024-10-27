#ifndef _SCREEN_GRAPHICS_H
#define _SCREEN_GRAPHICS_H

#include <stdint.h>

#define rgb(_r,_g,_b) ( ((_r) << 16 | (_g) << 8 | (_b)) & 0x00ffffff )

void drawPixel(int32_t x, int32_t y, uint32_t color);
void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
void drawRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
void drawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color);
void drawCircle(int32_t x, int32_t y, uint16_t radius, uint32_t color);
void boundaryFill(int32_t x, int32_t y, uint32_t fillColor, uint32_t boundaryColor);
void fillRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
void fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color);
void fillCircle(int32_t x, int32_t y, uint16_t radius, uint32_t color);

#endif

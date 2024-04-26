#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "stdint.h"
#include "screen.h"
#include "math.h"

// Basic full saturation colors
#define BLACK   0x00000000 // 0
#define RED     0x00ff0000 // 1
#define GREEN   0x0000ff00 // 2
#define YELLOW  0x00ffff00 // 3
#define BLUE    0x000000ff // 4
#define MAGENTA 0x00ff00ff // 5
#define CYAN    0x0000ffff // 6
#define WHITE   0x00ffffff // 7

#define rgb(r,g,b) ( ((r) << 16 | (g) << 8 | (b)) & 0x00ffffff )

void drawPixel(int32_t x, int32_t y, uint32_t color) {
    uint32_t* vidmem = *(uint32_t**)(MODE_INFO_BLOCK + 40);
    
    uint32_t offset = (y * WIDTH) + x;
    vidmem[offset] = color;
}

// Source: https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) {
    int32_t dx = abs(x2 - x1);
    int32_t dy = -abs(y2 - y1);
    int8_t sx = (x1 < x2) ? 1 : -1;
    int8_t sy = (y1 < y2) ? 1 : -1;
    int16_t err = dx + dy;
    
    // TODO: Sometimes the condition isn't met for some reason and it breaks everything
    while (!(x1 == x2 && y1 == y2)) {
        drawPixel(x1, y1, color);

        if (err * 2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (err * 2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void drawRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) {
    drawLine(x1, y1, x2, y1, color);
    drawLine(x2, y1, x2, y2, color);
    drawLine(x2, y2, x1, y2, color);
    drawLine(x1, y2, x1, y1, color);
}

void drawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color) {
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x3, y3, color);
    drawLine(x3, y3, x1, y1, color);
}

void drawCircle(int32_t x, int32_t y, uint16_t radius, uint32_t color) {
    int16_t offX = 0;
    int16_t offY = radius;
    int16_t p = 1 - radius;

    // Draw 8 octant points
    drawPixel(x + offX, y + offY, color);
    drawPixel(x - offX, y + offY, color);
    drawPixel(x + offX, y - offY, color);
    drawPixel(x - offX, y - offY, color);
    drawPixel(x + offY, y + offX, color);
    drawPixel(x - offY, y + offX, color);
    drawPixel(x + offY, y - offX, color);
    drawPixel(x - offY, y - offX, color);

    while (offX < offY) {
        offX++;
        if (p < 0) p += 2 * offX + 1;
        else {
            offY--;
            p += 2 * (offX - offY) + 1;
        }
        
        drawPixel(x + offX, y + offY, color);
        drawPixel(x - offX, y + offY, color);
        drawPixel(x + offX, y - offY, color);
        drawPixel(x - offX, y - offY, color);
        drawPixel(x + offY, y + offX, color);
        drawPixel(x - offY, y + offX, color);
        drawPixel(x + offY, y - offX, color);
        drawPixel(x - offY, y - offX, color);
    }
}

// Recursive (may need large stack)
void boundaryFill(int32_t x, int32_t y, uint32_t fillColor, uint32_t boundaryColor) {
    uint32_t* vidmem = *(uint32_t**)(MODE_INFO_BLOCK + 40);

    uint32_t offset = y * WIDTH + x;

    if (vidmem[offset] != fillColor && vidmem[offset] != boundaryColor) {
        vidmem[offset] = fillColor;

        boundaryFill(x + 1, y, fillColor, boundaryColor);
        boundaryFill(x - 1, y, fillColor, boundaryColor);
        boundaryFill(x, y + 1, fillColor, boundaryColor);
        boundaryFill(x, y - 1, fillColor, boundaryColor);
    }
}

void fillRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) {
    int32_t cx = (x1 + x2) / 2;
    int32_t cy = (y1 + y2) / 2;

    drawRect(x1, y1, x2, y2, color - 1);
    boundaryFill(cx, cy, color, color - 1);
    drawRect(x1, y1, x2, y2, color);
}

void fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color) {
    int32_t cx = (x1 + x2 + x3) / 3;
    int32_t cy = (y1 + y2 + y3) / 3;

    drawTriangle(x1, y1, x2, y2, x3, y3, color - 1);
    boundaryFill(cx, cy, color, color - 1);
    drawTriangle(x1, y1, x2, y2, x3, y3, color);
}

void fillCircle(int32_t x, int32_t y, uint16_t radius, uint32_t color) {
    drawCircle(x, y, radius, color - 1);
    boundaryFill(x, y, color, color - 1);
    drawCircle(x, y, radius, color);
}

#endif

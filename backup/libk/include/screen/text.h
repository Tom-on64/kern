#ifndef _SCREEN_TEXT_H
#define _SCREEN_TEXT_H

#include <stdint.h>

typedef union {
    struct {
        uint8_t width;
        uint8_t height;
    };
    uint8_t data[4096];
} __attribute__ ((packed)) font_t;

void drawChar(char c, uint32_t x, uint32_t y, uint32_t fgColor, uint32_t bgColor);
void scroll();
void clearScreen(uint32_t bgColor);

#endif

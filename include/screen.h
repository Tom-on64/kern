#ifndef SCREEN_H
#define SCREEN_H

#include "stdint.h"

#define MODE_INFO_BLOCK 0x5000
#define BG_COLOR 0x00010b17
#define FG_COLOR 0x00ebddf4
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16

typedef struct modeInfoBlock_s {
    // Required by all VBE revisions
    uint16_t modeAttr;
    uint8_t windowAAtrr;
    uint8_t windowBAttr;
    uint16_t windowGranularity;
    uint16_t windowSize;
    uint16_t windowASegment;
    uint16_t windowBSegment;
    uint32_t windowFunctionPtr;
    uint16_t bytesPerScanline;

    // Required by VBE 1.2+
    uint16_t xRes;
    uint16_t yRes;
    uint8_t xCharSize;
    uint8_t yCharSize;
    uint8_t planeCount;
    uint8_t bpp;
    uint8_t bankCount;
    uint8_t memoryModel;
    uint8_t bankSize;
    uint8_t imagePageCount;
    uint8_t reserved1;

    // Direct color fields
    uint8_t redMaskSize;
    uint8_t redFieldPos;
    uint8_t greenMaskSize;
    uint8_t greenFieldPos;
    uint8_t blueMaskSize;
    uint8_t blueFieldPos;
    uint8_t reservedMaskSize;
    uint8_t reservedFieldPos;
    uint8_t directColorModeInfo;

    // Required by VBE 2.0+
    uint32_t physicalBasePtr;
    uint32_t reserved2;
    uint16_t reserved3;

    // Required by VBE 3.0+
    uint16_t linearBytesPerScanline;
    uint8_t bankImagePageCount;
    uint8_t linearImagePageCount;
    uint8_t linearRedMaskSize;
    uint8_t linearRedFieldPos;
    uint8_t linearGreenMaskSize;
    uint8_t linearGreenFieldPos;
    uint8_t linearBlueMaskSize;
    uint8_t linearBlueFieldPos;
    uint8_t linearReservedMaskSize;
    uint8_t linearReservedFieldPos;
    uint32_t maxPixelClock;

    uint8_t reserved4[190];
} __attribute__ ((packed)) modeInfoBlock_t;

struct cursor_s {
    uint32_t x;
    uint32_t y;
};

static char* font = (char*)0x6000;
static struct cursor_s cursor = { 0, 0 };
static modeInfoBlock_t* gfxMode = (modeInfoBlock_t*)MODE_INFO_BLOCK;

uint32_t bgColor = BG_COLOR;
uint32_t fgColor = FG_COLOR;

uint32_t convertColor(uint32_t color) {
    if (gfxMode->bpp == 8) { // Uses VGA Color pallete, idk how to support that
        return color & 0xff; 
    }

    uint8_t red   = ((color >> 16) & 0xff) * (((1 << gfxMode->linearRedMaskSize)   - 1) / 255.0);
    uint8_t green = ((color >> 8 ) & 0xff) * (((1 << gfxMode->linearGreenMaskSize) - 1) / 255.0);
    uint8_t blue  = ((color      ) & 0xff) * (((1 << gfxMode->linearBlueMaskSize)  - 1) / 255.0);

    return (red << gfxMode->redFieldPos) | (green << gfxMode->greenFieldPos) | (blue << gfxMode->blueFieldPos);
}

// TODO: Reimplement larger text
// TODO: Support different bpps (and still keep the colors the same)
void putcAt(unsigned char c, uint32_t x, uint32_t y) {
    uint8_t* vidmem = (uint8_t*)gfxMode->physicalBasePtr;
    uint8_t bytesPerPx = (gfxMode->bpp+1) / 8;
    uint32_t offset = (gfxMode->xRes * y * CHAR_HEIGHT + x * CHAR_WIDTH) * bytesPerPx;

    char* bitmap = &font[((c % 128) - 1) * 16];
    for (uint8_t i = 0; i < CHAR_HEIGHT; i++) { // Rows
        char bitmapRow = bitmap[i]; 
        
        for (uint8_t j = 0; j < CHAR_WIDTH * bytesPerPx; ++j) { // Cols 
            uint32_t color = convertColor((bitmapRow & 0x80) ? fgColor : bgColor);
            uint8_t byte = j % bytesPerPx;
            vidmem[offset + j] = (uint8_t)(color >> (8 * byte));

            if (byte == bytesPerPx - 1) {
                bitmapRow = bitmapRow << 1;
            }
        }

        offset += gfxMode->linearBytesPerScanline; // gfxMode->xRes * bytesPerPx
    }
}

void printAt(const char* str, uint32_t x, uint32_t y) {
    while (*str != '\0') {
        putcAt(*str++, x, y);
        x++;
    }
}

void putc(char c) {
    if (c == '\n') {
        putcAt(' ', cursor.x, cursor.y); // Make sure we don't leave an extra cursor
        cursor.y++;
        cursor.x = 0;
    } else if (c == '\b') {
        putcAt(' ', cursor.x, cursor.y); // Make sure we don't leave an extra cursor TODO: make this better
        cursor.x--;
        putcAt(' ', cursor.x, cursor.y);
    } else {
        putcAt(c, cursor.x, cursor.y);
        cursor.x++;
    }
}

void print(const char* str) {
    while (*str != '\0') {
        putc(*str++);
    }
}

// TODO: Optimize this, it is slow (i made it slightly better)
void clear() {
    uint8_t* vidmem = (uint8_t*)gfxMode->physicalBasePtr;
    uint8_t bytesPerPx = (gfxMode->bpp+1) / 8;

    cursor.x = 0;
    cursor.y = 0;

    uint32_t color = convertColor(bgColor);
   
    for (uint32_t i = 0; i < gfxMode->xRes * gfxMode->yRes; i++) {
        for (uint8_t j = 0; j < bytesPerPx; j++) {
            vidmem[j] = (uint8_t)(color >> (j * 8));
        }
        vidmem += bytesPerPx;
    }
}

#endif

#ifndef SCREEN_H
#define SCREEN_H

#include "stdint.h"

#define MODE_INFO_BLOCK 0x5000
#define BG_COLOR 0xff010b17
#define FG_COLOR 0xffebddf4

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

// TODO: Probably make the font bigger in a better way, but this works for now
void putcAt(unsigned char c, uint32_t x, uint32_t y) {
    uint32_t* vidmem = (uint32_t*)gfxMode->physicalBasePtr;
    uint32_t offset = (gfxMode->xRes * y * 16 + x * 8) * 2;

    char* bitmap = &font[((c % 128) - 1) * 16];
    for (uint8_t i = 0; i < 32; i++) { // Rows (16*2 since we draw font 2x larger)
        char bitmapRow = bitmap[i/2]; // Idk if this should work, because i'm scared of division
        
        for (uint8_t j = 0; j < 16; j++) { // Cols (8*2 same as rows)
            vidmem[offset + j] = (bitmapRow & 0x80) ? FG_COLOR : BG_COLOR;
            if (j % 2) {
                bitmapRow = bitmapRow << 1;
            }
        } 

        offset += gfxMode->xRes;
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

void clear() {
    uint32_t* vidmem = (uint32_t*)gfxMode->physicalBasePtr;

    size_t i = 0;
    while (i < gfxMode->xRes * gfxMode->yRes) {
        vidmem[i] = BG_COLOR; // #1f2937
        i++;
    }

    cursor.x = 0;
    cursor.y = 0;
}

#endif

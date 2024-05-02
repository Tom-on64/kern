#ifndef SCREEN_H
#define SCREEN_H

#include "stdint.h"
#include "string.h"

#define MODE_INFO_BLOCK 0x9000
#define BG_COLOR 0x00010b17
#define FG_COLOR 0x00ebddf4

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
void putcAt(unsigned char c, uint32_t x, uint32_t y) {
    // Font info
    uint8_t charWidth = *(font);
    uint8_t charHeight = *(font + 1);
    // uint8_t bitmapRowLen = (charWidth - 1) / 8 + 1; // TODO: Larger font support
    
    // Video stuff
    uint8_t bytesPerPx = (gfxMode->bpp+1) / 8;
    uint8_t* vidmem = (uint8_t*)gfxMode->physicalBasePtr;
    uint32_t offset = (gfxMode->xRes * y * charHeight + x * charWidth) * bytesPerPx;

    char* bitmap = &font[((c % 128) - 1) * charHeight];
    for (uint8_t i = 0; i < charHeight; i++) { // Rows
        char bitmapRow = bitmap[i];

        for (uint8_t j = 0; j < charHeight * bytesPerPx; ++j) { // Cols 
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

void scroll() { // TODO: Make this faster (still kinda slow, but memcpy32() helped alot)
    uint8_t charHeight = *(font + 1);
    if ((cursor.y+1) * charHeight < gfxMode->yRes) { return; } // Check if we should scroll

    uint32_t bytesPerLine = charHeight * gfxMode->bytesPerScanline;
    uint32_t lines = gfxMode->yRes / charHeight;
    uint32_t vidmem = gfxMode->physicalBasePtr;

    for (uint32_t i = 1; i < lines; i++) {
        uint32_t address = vidmem + i * bytesPerLine;
        memcpy32((char*)address, (char*)(address - bytesPerLine), bytesPerLine);
    }
    
    uint32_t color = convertColor(bgColor);
    uint8_t* lastLine = (uint8_t*)(vidmem + (lines-1) * bytesPerLine);
    uint8_t bytesPerPx = (gfxMode->bpp+1) / 8;
    
    for (uint32_t i = 0; i < bytesPerLine; i++) {
        for (uint8_t j = 0; j < bytesPerPx; j++) {
            lastLine[j] = (uint8_t)(color >> (j * 8));
        }
        lastLine += bytesPerPx;
    }

    cursor.y--;
}

void putc(char c) {
    uint8_t charWidth = *(font);

    if (c == '\n') {
        putcAt(' ', cursor.x, cursor.y); // Make sure we don't leave an extra cursor
        cursor.y++;
        cursor.x = 0;
    } else if (c == '\b') {
        putcAt(' ', cursor.x, cursor.y); // Make sure we don't leave an extra cursor

        if (cursor.x == 0) {
            cursor.x = gfxMode->xRes / charWidth - 1; // Subtract one so we don't overflow to the line start
            cursor.y--; // y could be 0
        } else {
            cursor.x--;
        }

        putcAt(' ', cursor.x, cursor.y);
    } else {
        putcAt(c, cursor.x, cursor.y);
        cursor.x++;

        if (cursor.x >= gfxMode->xRes / charWidth) {
            cursor.y++;
            cursor.x = 0;
        }
    }

    scroll();
}

void print(const char* str) {
    while (*str != '\0') {
        putc(*str++);
    }
}

void clear() {
    uint8_t* vidmem = (uint8_t*)gfxMode->physicalBasePtr;
    uint8_t bytesPerPx = (gfxMode->bpp+1) / 8;

    cursor.x = 0;
    cursor.y = 0;

    uint32_t color = convertColor(bgColor);
   
    for (uint32_t i = 0; i < gfxMode->xRes; i++) { // Clear one line of pixels
        for (uint8_t j = 0; j < bytesPerPx; j++) {
            vidmem[j] = (uint8_t)(color >> (j * 8));
        }
        vidmem += bytesPerPx;
    }

    vidmem -= gfxMode->bytesPerScanline;

    for (uint32_t i = 0; i < gfxMode->yRes; i++) { // Copy that line to clear the entire screen (using memcpy32 is fast!)
        memcpy32(vidmem, vidmem + gfxMode->bytesPerScanline, gfxMode->bytesPerScanline);
        vidmem += gfxMode->bytesPerScanline;
    }
}

#endif

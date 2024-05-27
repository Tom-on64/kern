#ifndef SCREEN_H
#define SCREEN_H

#include <memory/addresses.h>
#include <screen/gfxmode.h>
#include <stdint.h>
#include <string.h>

struct cursor_s {
    uint32_t x;
    uint32_t y;
};

static char* font = (char*)FONT_LOC;
static struct cursor_s cursor = { 0, 0 };

uint32_t bgColor = BG_COLOR;
uint32_t fgColor = FG_COLOR;

uint32_t convertColor(uint32_t color) {
    if (gfxMode->bpp == 8) { // Uses VGA Color pallete, idk how to support that
        return color & 0xff; 
    }

    // TODO: !! Important !! - Is should be divided by 255.0 (for floats), but using '-mgeneral-regs-only' removes fp-registers and
    //                         wants to use some dumb functions for converting fp numbers in the stdlib (that we don't have)
    uint8_t red   = ((color >> 16) & 0xff) * (((1 << gfxMode->linearRedMaskSize)   - 1) / 255);
    uint8_t green = ((color >> 8 ) & 0xff) * (((1 << gfxMode->linearGreenMaskSize) - 1) / 255);
    uint8_t blue  = ((color      ) & 0xff) * (((1 << gfxMode->linearBlueMaskSize)  - 1) / 255);

    return (red << gfxMode->redFieldPos) | (green << gfxMode->greenFieldPos) | (blue << gfxMode->blueFieldPos);
}

// TODO: Reimplement larger text
void putcAt(unsigned char c, uint32_t x, uint32_t y) {
    // Font info
    uint8_t charWidth = *(uint8_t*)FONT_WIDTH;
    uint8_t charHeight = *(uint8_t*)FONT_HEIGHT;
    // uint8_t bitmapRowLen = (charWidth - 1) / 8 + 1; // TODO: Larger font support
    
    // Video stuff
    uint8_t bytesPerPx = (gfxMode->bpp+1) / 8;
    uint8_t* vidmem = (uint8_t*)gfxMode->physicalBasePtr;
    uint32_t offset = (gfxMode->xRes * y * charHeight + x * charWidth) * bytesPerPx;

    char* bitmap = &font[((c % 128) - 1) * charHeight];
    for (uint8_t i = 0; i < charHeight; i++) { // Rows
        char bitmapRow = bitmap[i];
        uint32_t color = convertColor((bitmapRow & 0x80) ? fgColor : bgColor);

        for (uint8_t j = 0; j < charWidth * bytesPerPx; ++j) { // Cols 
            uint8_t byte = j % bytesPerPx;
            vidmem[offset + j] = (uint8_t)(color >> (8 * byte));

            if (byte == bytesPerPx - 1) {
                bitmapRow = bitmapRow << 1;
                color = convertColor((bitmapRow & 0x80) ? fgColor : bgColor);
            }
        }

        offset += gfxMode->linearBytesPerScanline; // gfxMode->xRes * bytesPerPx
    }
}

// TODO: Probably get rid of this, idk (use terminal control codes)
void printAt(const char* str, uint32_t x, uint32_t y) {
    while (*str != '\0') {
        putcAt(*str++, x, y);
        x++;
    }
}

// TODO: Use terminal.x/y instead of cursor.x/y
void scroll() { // TODO: Make this faster (still kinda slow, but memcpy32() helped alot)
    uint8_t charHeight = *(uint8_t*)FONT_HEIGHT;
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

// DEPRICATED - TODO: Remove this from everywhere so i can delete it
void putc(char c) {
    uint8_t charWidth = *(uint8_t*)FONT_WIDTH;

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

// DEPRICATED - TODO: Remove this everywhere it's used and delete it's definition here
void print(const char* str) {
    while (*str != '\0') {
        putc(*str++);
    }
}

// TODO: Make this comply with the new terminal.h thing!
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

#include <screen/text.h>
#include <tty/tty.h>
#include <memory/addresses.h>
#include <screen/gfxmode.h>
#include <stdint.h>
#include <string.h>

// TODO: Reimplement larger text
void drawChar(char c, uint32_t x, uint32_t y, uint32_t fgColor, uint32_t bgColor) {
    // Font info
    font_t* font = (font_t*)FONT_LOC;
    uint8_t charWidth = font->width;
    uint8_t charHeight = font->height;
    // uint8_t bitmapRowLen = (charWidth - 1) / 8 + 1; // TODO: Larger font support
    
    // Video stuff
    uint8_t bytesPerPx = (gfxMode->bpp+7) / 8;
    uint8_t* vidmem = (uint8_t*)gfxMode->physicalBasePtr;
    uint32_t offset = (gfxMode->xRes * y * charHeight + x * charWidth) * bytesPerPx;

    uint8_t* bitmap = &font->data[(c - 1) * charHeight];
    for (uint8_t i = 0; i < charHeight; i++) { // Rows
        uint8_t bitmapRow = bitmap[i];
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

void scroll() { // TODO: Make this faster (still kinda slow, but memcpy32() helped alot)
    uint8_t charHeight = ((font_t*)FONT_LOC)->height;
    if (terminal->y+1 < terminal->height) { return; } // Check if we should scroll

    uint32_t bytesPerLine = charHeight * gfxMode->bytesPerScanline;
    uint32_t lines = terminal->height;
    uint32_t vidmem = gfxMode->physicalBasePtr;

    for (uint32_t i = 1; i < lines; i++) {
        uint32_t address = vidmem + i * bytesPerLine;
        memcpy32((char*)address, (char*)(address - bytesPerLine), bytesPerLine);
    }
    
    uint32_t color = convertColor(terminal->bg);
    uint8_t* lastLine = (uint8_t*)(vidmem + (lines-1) * bytesPerLine);
    uint8_t bytesPerPx = (gfxMode->bpp+1) / 8;
    
    for (uint32_t i = 0; i < bytesPerLine; i++) {
        for (uint8_t j = 0; j < bytesPerPx; j++) {
            lastLine[j] = (uint8_t)(color >> (j * 8));
        }
        lastLine += bytesPerPx;
    }

    terminal->y--;
}

// TODO: Make this comply with the new terminal.h thing!
void clearScreen(uint32_t bgColor) {
    uint8_t* vidmem = (uint8_t*)gfxMode->physicalBasePtr;
    uint8_t bytesPerPx = (gfxMode->bpp+1) / 8;

    terminal->x = 0;
    terminal->y = 0;

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


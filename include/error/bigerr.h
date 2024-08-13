#ifndef BIGERR_H
#define BIGERR_H

/*
 * Something went VERY wrong. 
 * Eg. kernel.bin is missing, no filesystem, Virtual Memory failed, etc.
 */

#include <stdint.h>
#include <screen/gfxmode.h>

void rect(uint32_t c, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    uint8_t* vidmem = (uint8_t*)gfxMode->physicalBasePtr;
    uint8_t bytesPerPx = gfxMode->bpp / 8;
    uint32_t convColor = convertColor(c);

    for (uint32_t dx = 0; dx < w; dx++) {
        for (uint32_t dy = 0; dy < h; dy++) {
            uint32_t offset = (((y + dy) * gfxMode->xRes) + (x + dx)) * bytesPerPx;

            for (uint8_t i = 0; i < gfxMode->bpp; i++) {
                vidmem[offset + i] = (uint8_t)(convColor >> (i*8));
            }
        }
    }
}

void bigError(uint32_t err) {
    // Red BG
    rect(0x00ff0000, 0, 0, gfxMode->xRes, gfxMode->yRes);
    // E
    rect(0x00ffffff, 20, 10, 10, 50);
    rect(0x00ffffff, 30, 10, 30, 10);
    rect(0x00ffffff, 30, 30, 30, 10);
    rect(0x00ffffff, 30, 50, 30, 10);
    // R
    rect(0x00ffffff, 70, 10, 10, 50);
    rect(0x00ffffff, 70, 10, 30, 10);
    rect(0x00ffffff, 70, 30, 30, 10);
    rect(0x00ffffff, 90, 10, 10, 30);
    rect(0x00ffffff, 80, 40, 10, 10);
    rect(0x00ffffff, 90, 50, 10, 10);
    // R
    rect(0x00ffffff, 110, 10, 10, 50);
    rect(0x00ffffff, 110, 10, 30, 10);
    rect(0x00ffffff, 110, 30, 30, 10);
    rect(0x00ffffff, 130, 10, 10, 30);
    rect(0x00ffffff, 120, 40, 10, 10);
    rect(0x00ffffff, 130, 50, 10, 10);
    // O
    rect(0x00ffffff, 150, 10, 10, 50);
    rect(0x00ffffff, 170, 10, 10, 50);
    rect(0x00ffffff, 150, 10, 30, 10);
    rect(0x00ffffff, 150, 50, 30, 10);
    // R
    rect(0x00ffffff, 190, 10, 10, 50);
    rect(0x00ffffff, 190, 10, 30, 10);
    rect(0x00ffffff, 190, 30, 30, 10);
    rect(0x00ffffff, 210, 10, 10, 30);
    rect(0x00ffffff, 200, 40, 10, 10);
    rect(0x00ffffff, 210, 50, 10, 10);

    __asm__ volatile ("cli; hlt" :: "a"(err));
}

#endif

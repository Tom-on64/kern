#ifndef SCREEN_GFXMODE_H
#define SCREEN_GFXMODE_H

#include <memory/addresses.h>
#include <stdint.h>

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

static modeInfoBlock_t* gfxMode = (modeInfoBlock_t*)MODE_INFO_BLOCK_LOC;

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

#endif

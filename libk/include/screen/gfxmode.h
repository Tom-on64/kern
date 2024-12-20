#ifndef _SCREEN_GFXMODE_H
#define _SCREEN_GFXMODE_H

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

extern modeInfoBlock_t* gfxMode;

uint32_t convertColor(uint32_t color);

#endif

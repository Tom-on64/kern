#include <screen/gfxmode.h>
#include <memory/addresses.h>
#include <stdint.h>

modeInfoBlock_t* gfxMode = (modeInfoBlock_t*)MODE_INFO_BLOCK_LOC;

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


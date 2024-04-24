#include "stdint.h"

void main() {
    uint32_t wait = 0xfffffff;

    uint32_t* vidmem = *(uint32_t**)(0x5000 + 40);

    for (uint32_t x = 200; x < 400; x++) {
        for (uint32_t y = 200; y < 400; y++) {
            vidmem[y * 1920 + x] = 0x00ffffff;
        }
    }

    while (wait--);

    return;
}

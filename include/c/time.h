#ifndef TIME_H
#define TIME_H

#include <stdint.h>

void sleep(uint32_t ms) {
    __asm__ volatile ("int $0x80" : : "a"(0), "b"(ms));
}

#endif

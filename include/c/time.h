#ifndef TIME_H
#define TIME_H

#include <stdint.h>

void sleep(uint32_t ms) {
    // Call syscall 0 (eax) with 'ms' miliseconds (ebx)
    __asm__ volatile ("int $0x80" : : "a"(0), "b"(ms));
}

#endif

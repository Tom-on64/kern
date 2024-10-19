#include <string.h>
#include <stdint.h>

void* memcpy(void* src, void* dst, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        *((uint8_t*)dst + i) = *((uint8_t*)src + i);
    }
    return dst;
}

void* memcpy32(void* src, void* dst, uint32_t length) {
    for (uint32_t i = 0; i * 4 < length; i++) {
        *((uint32_t*)dst + i) = *((uint32_t*)src + i);
    }
    return dst;
}


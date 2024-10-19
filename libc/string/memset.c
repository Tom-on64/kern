#include <string.h>
#include <stdint.h>
#include <stddef.h>

void* memset(void* dst, uint8_t value, size_t length) {
    for (size_t i = 0; i < length; i++) {
        *(uint8_t*)(dst + i) = value;
    }
    return dst;
}

void* memset32(void* dst, uint32_t value, size_t length) {
    for (size_t i = 0; i < length; i++) {
        *(uint32_t*)(dst + i) = value;
    }
    return dst;
}


#include "system.h"
#include "stdint.h"

uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile("in %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outb(uint16_t port, uint8_t data) {
    asm volatile("out %0, %1" :: "a"(data), "Nd"(port));
}

char* memcopy(char* src, char* dst, uint16_t length) {
    for (int i = 0; i < length; i++) {
        *(dst + i) = *(src + i);
    }
    return dst;
}

char* memset(char* dst, char value, uint16_t length) {
    for (int i = 0; i < length; i++) {
        *(dst + i) = value;
    }
    return dst;
}

void cli() {
    asm volatile("cli");
}

void sti() {
    asm volatile("sti");
}

void hlt() {
    asm volatile("hlt");
}


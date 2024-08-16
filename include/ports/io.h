#ifndef PORTS_IO_H
#define PORTS_IO_H

#include <stdint.h>

// Port functions
uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outb(uint16_t port, uint8_t data) {
    __asm__ volatile("outb %0, %1" :: "a"(data), "Nd"(port));
}

uint16_t inw(uint16_t port) {
    uint16_t result;
    __asm__ volatile("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outw(uint16_t port, uint16_t data) {
    __asm__ volatile("outw %0, %1" :: "a"(data), "Nd"(port));
}

void ioWait() {
    // Waits ~1 I/O Cycle (Linux uses this so it should be good)
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
}

#endif

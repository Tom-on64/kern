#ifndef SYSTEM_H
#define SYSTEM_H

#include "stdint.h"

// Port functions
uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outb(uint16_t port, uint8_t data) {
    asm volatile("outb %0, %1" :: "a"(data), "Nd"(port));
}

uint16_t inw(uint16_t port) {
    uint16_t result;
    asm volatile("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outw(uint16_t port, uint16_t data) {
    asm volatile("outw %0, %1" :: "a"(data), "Nd"(port));
}


// Assembly stuff
void cli() {
    asm volatile("cli");
}

void sti() {
    asm volatile("sti");
}

void hlt() {
    asm volatile("hlt");
}

#endif

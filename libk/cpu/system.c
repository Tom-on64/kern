#include <cpu/system.h>
#include <drivers/serial.h>
#include <stdint.h>

void cpuid(uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
    __asm__ volatile (
        "cpuid" 
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(*eax)
    );
}

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

uint32_t inl(uint16_t port) {
    uint32_t result;
    __asm__ volatile("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
void outl(uint16_t port, uint32_t data) {
    __asm__ volatile("outl %0, %1" :: "a"(data), "Nd"(port));
}

void ioWait() {
    // Waits ~1 I/O Cycle (Linux uses this so it should be good)
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
}

void panic() {
    debugf("\x1b[31m[kernel] Kern panic!\x1b[0m\n");
    while (1) { __asm__ volatile ("cli; hlt"); }
}


#ifndef PORT_H
#define PORT_H

#include "stdint.h"

inline uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
inline void outb(uint16_t port, uint8_t data) { 
    __asm__ volatile("outb %0, %1" :: "a"(data), "Nd"(port));
}

inline uint16_t inw(uint16_t port) {
    uint16_t result;
    __asm__ volatile("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
inline void outw(uint16_t port, uint16_t data) {
    __asm__ volatile("outw %0, %1" :: "a"(data), "Nd"(port));
}

inline uint32_t inl(uint16_t port) {
    uint32_t result;
    __asm__ volatile("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
inline void outl(uint16_t port, uint32_t data) {
    __asm__ volatile("outl %0, %1" :: "a"(data), "Nd"(port));
}

#endif

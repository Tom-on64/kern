#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <kernel.h>

void cpuid(uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);

// Ports
uint8_t  inb(uint16_t port);
void     outb(uint16_t port, uint8_t data);
uint16_t inw(uint16_t port);
void     outw(uint16_t port, uint16_t data);
uint32_t inl(uint16_t port);
void     outl(uint16_t port, uint32_t data);

#endif

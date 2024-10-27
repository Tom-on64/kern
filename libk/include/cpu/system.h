#ifndef _CPU_SYSTEM_H
#define _CPU_SYSTEM_H

#include <stdint.h>

void cpuid(uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);

void outb(uint16_t port, uint8_t data);
void outw(uint16_t port, uint16_t data);
void outl(uint16_t port, uint32_t data);

uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t inl(uint16_t port);

void ioWait();
void panic();

#endif

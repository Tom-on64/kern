#ifndef SYSTEM_H
#define SYSTEM_H

#include "stdint.h"

// Port functions
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t data);

// Memory functions
char* memcopy(char* src, char* dst, uint16_t length);
char* memset(char* dst, char data, uint16_t length);

struct regs {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int intNo, errCode;
    unsigned int eip, cs, eflags, useresp, ss;
};

#endif

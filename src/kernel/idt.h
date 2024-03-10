#ifndef IDT_H
#define IDT_H

#include "stdint.h"

void setupIdt();
void idtSetGate(uint8_t i, uint32_t offset, uint16_t selector, uint8_t typeAttr);
extern void idtLoad();

// Contains values of registers and exception info from isrs
struct regs {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int intNo, errCode;
    unsigned int eip, cs, eflags, useresp, ss;
};

#endif


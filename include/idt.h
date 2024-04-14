#ifndef IDT_H
#define IDT_H

#include "stdint.h"
#include "system.h"

struct idtEntry {
    uint16_t offsetLow;
    uint16_t selector;
    uint8_t zero;
    uint8_t typeAttr;
    uint16_t offsetHigh;
} __attribute__((packed));

struct idtPointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idtEntry idt[256] = { 0 };
struct idtPointer idtp;

extern void idtLoad();

void idtSetGate(uint8_t i, uint32_t offset, uint16_t selector, uint8_t typeAttr) {
    idt[i].offsetLow = (uint16_t)offset;
    idt[i].selector = selector;
    idt[i].zero = 0;
    idt[i].typeAttr = typeAttr;
    idt[i].offsetHigh = (uint16_t)(offset >> 16);
}

void setupIdt() {
    idtp.limit = sizeof(struct idtEntry) * 256;
    idtp.base = (uint32_t)&idt;

    idtLoad();
}

// Contains values of registers and exception info from isrs
struct regs {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int intNo, errCode;
    unsigned int eip, cs, eflags, useresp, ss;
};

#endif


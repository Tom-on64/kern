#include "idt.h"
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
    sti();
}


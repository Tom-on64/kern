#include <interrupt/idt.h>
#include <stdint.h>

#define IDT_ENTRIES 256

__attribute__ ((aligned(0x10))) // For performance
idtEntry_t idt[IDT_ENTRIES] = { 0 };
idtp_t idtp;

void idtSetGate(uint8_t i, void* handler, uint8_t flags) {
    uint32_t offset = (uint32_t)handler;
    idtEntry_t* entry = &idt[i];

    entry->offsetLow = (uint16_t)offset;
    entry->selector = 0x08;
    entry->zero = 0;
    entry->typeAttr = flags;
    entry->offsetHigh = (uint16_t)(offset >> 16);
}

void setupIdt() {
    idtp.limit = sizeof(idtEntry_t) * 256;
    idtp.base = (uint32_t)&idt;

    __asm__ volatile ("lidt %0" : : "m"(idtp));
}


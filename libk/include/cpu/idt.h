#ifndef _CPU_IDT_H
#define _CPU_IDT_H

#include <stdint.h>

#define IDT_ENTRIES 256

#define TRAP_GATE_FLAGS    0x8f // P = 1, DPL = 00, S = 0, Type = 1111 (32-bit Trap gate)
#define INT_GATE_FLAGS     0x8e // P = 1, DPL = 00, S = 0, Type = 1110 (32-bit Int gate)
#define USR_INT_GATE_FLAGS 0xee // P = 1, DPL = 11, S = 0, Type = 1110 (32-bit Int gate)

typedef struct {
    uint16_t offsetLow;
    uint16_t selector;
    uint8_t zero;
    uint8_t typeAttr;
    uint16_t offsetHigh;
} __attribute__ ((packed)) idtEntry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__ ((packed)) idtp_t;

void idtSetGate(uint8_t i, void* handler, uint8_t flags);
void setupIdt();

#endif

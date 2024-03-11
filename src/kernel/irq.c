#include "irq.h"
#include "stdint.h"
#include "system.h"

void* irqRoutines[16] = { 0 };

void installIrqHandler(uint32_t irq, void (*handler)(struct regs* r)) {
    irqRoutines[irq] = handler;
}

void uninstallIrqHandler(uint32_t irq) {
    irqRoutines[irq] = 0;
}

// I have no clue how or what this does, but it makes everything work
void irqRemap(void) {
    outb(0xa0, 0x11);
    outb(0x20, 0x11);
    outb(0x21, 0x20);
    outb(0xa1, 0x28);
    outb(0x21, 0x04);
    outb(0xa1, 0x02);
    outb(0x21, 0x01);
    outb(0xa1, 0x01);
    outb(0x21, 0x0);
    outb(0xa1, 0x0);
}

void irqHandler(struct regs* r) {
    void (*handler)(struct regs* r);

    handler = irqRoutines[r->intNo - 32]; // Offset by 32 because of isrs
    if (handler) handler(r);

    // Something about master and slave handlers
    if (r->intNo >= 40) outb(0xa0, 0x20);
    outb(0x20, 0x20);
}

void setupIrqs() {
    irqRemap();

    // Same as isrs, but differnet handlers :o
    idtSetGate(32, (unsigned)irq0, 0x08, 0x8e);
    idtSetGate(33, (unsigned)irq1, 0x08, 0x8e);
    idtSetGate(34, (unsigned)irq2, 0x08, 0x8e);
    idtSetGate(35, (unsigned)irq3, 0x08, 0x8e);
    idtSetGate(36, (unsigned)irq4, 0x08, 0x8e);
    idtSetGate(37, (unsigned)irq5, 0x08, 0x8e);
    idtSetGate(38, (unsigned)irq6, 0x08, 0x8e);
    idtSetGate(39, (unsigned)irq7, 0x08, 0x8e);
    idtSetGate(40, (unsigned)irq8, 0x08, 0x8e);
    idtSetGate(41, (unsigned)irq9, 0x08, 0x8e);
    idtSetGate(42, (unsigned)irq10, 0x08, 0x8e);
    idtSetGate(43, (unsigned)irq11, 0x08, 0x8e);
    idtSetGate(44, (unsigned)irq12, 0x08, 0x8e);
    idtSetGate(45, (unsigned)irq13, 0x08, 0x8e);
    idtSetGate(46, (unsigned)irq14, 0x08, 0x8e);
    idtSetGate(47, (unsigned)irq15, 0x08, 0x8e);
}


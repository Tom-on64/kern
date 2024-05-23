#ifndef ISRS_H
#define ISRS_H

#include <interrupt/idt.h>
#include <screen/text.h>

// Exception handlers
__attribute__ ((interrupt))
void excp_divideErr(intFrame_t* iframe) { // ISR 0
    fgColor = RED;
    print("[ Division Error ]\n");
    iframe->eip++;
}

__attribute__ ((interrupt))
void excp_doubleFault(intFrame_t* iframe, uint32_t errCode) { // ISR 8
    fgColor = RED;
    print("[ Double Fault ]\n"); // Error code should be 0 (TODO: Maybe check if it's != 0?)
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_pageFault(intFrame_t* iframe, uint32_t errCode) { // ISR 14
    uint32_t badAddress = 0;

    // cr2 should contain invalid address
    __asm__ volatile("movl %%cr2, %0" : "=r"(badAddress));

    fgColor = RED;
    print("[ Page Fault : 0x");
    print(itoa(errCode, 16));
    print(" ] - Tried to access 0x");
    print(itoa(badAddress, 16));
    print(".\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_reserved(intFrame_t* iframe) { // ISR 15
    fgColor = RED;
    print("[ Unknown Exception ]\n");
    __asm__ volatile ("cli; hlt");
}

// TODO: Add more (because there's always more)

// Repetative isr definitions
void setupExceptions() {
    for (uint8_t i = 0; i < 32; i++) {
        idtSetGate(i, excp_reserved, TRAP_GATE_FLAGS); // Doesn't account for err codes, will just halt anyways
    }

    idtSetGate(0,  excp_divideErr, TRAP_GATE_FLAGS);
    idtSetGate(8,  excp_doubleFault, TRAP_GATE_FLAGS);
    idtSetGate(14, excp_pageFault, TRAP_GATE_FLAGS);
    idtSetGate(15, excp_reserved, TRAP_GATE_FLAGS);
}

#endif

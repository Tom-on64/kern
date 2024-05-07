#ifndef ISRS_H
#define ISRS_H

#include <interrupt/idt.h>
#include <screen/text.h>

// Exception handlers
__attribute__ ((interrupt))
void excp_divideErr(intFrame_t* iframe) { // ISR 0
    fgColor = RED;
    print("[ Division Error ]\n");
    __asm__ volatile ("cli; hlt"); // Halt system
}

__attribute__ ((interrupt))
void excp_debugErr(intFrame_t* iframe) { // ISR 1
    fgColor = RED;
    print("[ Debug Exception ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_nmiInterrupt(intFrame_t* iframe) { // ISR 2
    fgColor = RED;
    print("[ Non-maskable Interrupt ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_breakpoint(intFrame_t* iframe) { // ISR 3
    fgColor = RED;
    print("[ Breakpoint ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_overflow(intFrame_t* iframe) { // ISR 4
    fgColor = RED;
    print("[ Overflow Error ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_boundRangeExceed(intFrame_t* iframe) { // ISR 5
    fgColor = RED;
    print("[ Bound Range Exceeded ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt)) 
void excp_invalidOpcode(intFrame_t* iframe) { // ISR 6
    fgColor = RED;
    print("[ Invalid OpCode ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_deviceNA(intFrame_t* iframe) { // ISR 7
    fgColor = RED;
    print("[ Device Not Available ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_doubleFault(intFrame_t* iframe, uint32_t errCode) { // ISR 8
    fgColor = RED;
    print("[ Double Fault : 0x");
    print(itoa(errCode, 16));
    print(" ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_coprocOverrun(intFrame_t* iframe) { // ISR 9
    fgColor = RED;
    print("[ Coprocessor Segment Overrun ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_invalidTss(intFrame_t* iframe, uint32_t errCode) { // ISR 10
    fgColor = RED;
    print("[ Invalid TSS : 0x");
    print(itoa(errCode, 16));
    print(" ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_noSegment(intFrame_t* iframe, uint32_t errCode) { // ISR 11
    fgColor = RED;
    print("[ Segment Not Present : 0x");
    print(itoa(errCode, 16));
    print(" ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_stackSegmentFault(intFrame_t* iframe, uint32_t errCode) { // ISR 12
    fgColor = RED;
    print("[ Stack-Segment Fault : 0x");
    print(itoa(errCode, 16));
    print(" ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_generalProtectionFault(intFrame_t* iframe, uint32_t errCode) { // ISR 13
    fgColor = RED;
    print("[ General Protection Fault : 0x");
    print(itoa(errCode, 16));
    print(" ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_pageFault(intFrame_t* iframe, uint32_t errCode) { // ISR 14
    fgColor = RED;
    print("[ Page Fault : 0x");
    print(itoa(errCode, 16));
    print(" ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_reserved(intFrame_t* iframe) { // ISR 15
    fgColor = RED;
    print("[ Reserved Exception ]\n"); // Shouldn't get this one
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_floatingPointErr(intFrame_t* iframe) { // ISR 16
    fgColor = RED;
    print("[ x87 FPU Floating-Point Error ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_alignCheck(intFrame_t* iframe, uint32_t errCode) { // ISR 17
    fgColor = RED;
    print("[ Alignment Check : 0x");
    print(itoa(errCode, 16));
    print(" ]\n");
    __asm__ volatile ("cli; hlt");
}

__attribute__ ((interrupt))
void excp_machineCheck(intFrame_t* iframe) { // ISR 18
    fgColor = RED;
    print("[ Machine Check ]\n");
    __asm__ volatile ("cli; hlt");
}

// TODO: Add more (because there's always more)

// Repetative isr definitions
void setupExceptions() {
    for (uint8_t i = 0; i < 32; i++) {
        idtSetGate(i, excp_reserved, TRAP_GATE_FLAGS);
    }

    idtSetGate(0,  excp_divideErr, TRAP_GATE_FLAGS);
    idtSetGate(1,  excp_debugErr, TRAP_GATE_FLAGS);
    idtSetGate(2,  excp_nmiInterrupt, TRAP_GATE_FLAGS);
    idtSetGate(3,  excp_breakpoint, TRAP_GATE_FLAGS);
    idtSetGate(4,  excp_overflow, TRAP_GATE_FLAGS);
    idtSetGate(5,  excp_boundRangeExceed, TRAP_GATE_FLAGS);
    idtSetGate(6,  excp_invalidOpcode, TRAP_GATE_FLAGS);
    idtSetGate(7,  excp_deviceNA, TRAP_GATE_FLAGS);
    idtSetGate(8,  excp_doubleFault, TRAP_GATE_FLAGS);
    idtSetGate(9,  excp_coprocOverrun, TRAP_GATE_FLAGS);
    idtSetGate(10, excp_invalidTss, TRAP_GATE_FLAGS);
    idtSetGate(11, excp_noSegment, TRAP_GATE_FLAGS);
    idtSetGate(12, excp_stackSegmentFault, TRAP_GATE_FLAGS);
    idtSetGate(13, excp_generalProtectionFault, TRAP_GATE_FLAGS);
    idtSetGate(14, excp_pageFault, TRAP_GATE_FLAGS);
    idtSetGate(15, excp_reserved, TRAP_GATE_FLAGS);
    idtSetGate(16, excp_floatingPointErr, TRAP_GATE_FLAGS);
    idtSetGate(17, excp_alignCheck, TRAP_GATE_FLAGS);
    idtSetGate(18, excp_machineCheck, TRAP_GATE_FLAGS);
    // Other are reserved
}

#endif

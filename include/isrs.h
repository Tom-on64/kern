#ifndef ISRS_H
#define ISRS_H

#include "idt.h"
#include "screen.h"
#include "system.h"
#include "string.h"

char* exceptionMessages[] = {
    "Divide By Zero Error",
    "Debug Exception",
    "NMI Interrupt",
    "Breakpoint",
    "Overflow",
    "BOUND Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 FPU Floating-Point Error",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};


// Repetative isr definitions
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

void setupIsrs() {
    idtSetGate(0, (unsigned)isr0, 0x08, 0x8e);
    idtSetGate(1, (unsigned)isr1, 0x08, 0x8e);
    idtSetGate(2, (unsigned)isr2, 0x08, 0x8e);
    idtSetGate(3, (unsigned)isr3, 0x08, 0x8e);
    idtSetGate(4, (unsigned)isr4, 0x08, 0x8e);
    idtSetGate(5, (unsigned)isr5, 0x08, 0x8e);
    idtSetGate(6, (unsigned)isr6, 0x08, 0x8e);
    idtSetGate(7, (unsigned)isr7, 0x08, 0x8e);
    idtSetGate(8, (unsigned)isr8, 0x08, 0x8e);
    idtSetGate(9, (unsigned)isr9, 0x08, 0x8e);
    idtSetGate(10, (unsigned)isr10, 0x08, 0x8e);
    idtSetGate(11, (unsigned)isr11, 0x08, 0x8e);
    idtSetGate(12, (unsigned)isr12, 0x08, 0x8e);
    idtSetGate(13, (unsigned)isr13, 0x08, 0x8e);
    idtSetGate(14, (unsigned)isr14, 0x08, 0x8e);
    idtSetGate(15, (unsigned)isr15, 0x08, 0x8e);
    idtSetGate(16, (unsigned)isr16, 0x08, 0x8e);
    idtSetGate(17, (unsigned)isr17, 0x08, 0x8e);
    idtSetGate(18, (unsigned)isr18, 0x08, 0x8e);
    idtSetGate(19, (unsigned)isr19, 0x08, 0x8e);
    idtSetGate(20, (unsigned)isr20, 0x08, 0x8e);
    idtSetGate(21, (unsigned)isr21, 0x08, 0x8e);
    idtSetGate(22, (unsigned)isr22, 0x08, 0x8e);
    idtSetGate(23, (unsigned)isr23, 0x08, 0x8e);
    idtSetGate(24, (unsigned)isr24, 0x08, 0x8e);
    idtSetGate(25, (unsigned)isr25, 0x08, 0x8e);
    idtSetGate(26, (unsigned)isr26, 0x08, 0x8e);
    idtSetGate(27, (unsigned)isr27, 0x08, 0x8e);
    idtSetGate(28, (unsigned)isr28, 0x08, 0x8e);
    idtSetGate(29, (unsigned)isr29, 0x08, 0x8e);
    idtSetGate(30, (unsigned)isr30, 0x08, 0x8e);
    idtSetGate(31, (unsigned)isr31, 0x08, 0x8e);
}

void faultHandler(struct regs* r) {
    if (r->intNo < 32) {
        print(exceptionMessages[r->intNo]);

        if (r->errCode) { 
            print(": 0x");
            print(itoa(r->errCode, 16));
        }

        cli();
        hlt();
    } 
}

#endif

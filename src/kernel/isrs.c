#include "isrs.h"
#include "idt.h"
#include "screen.h"
#include "system.h"

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
        print(exceptionMessages[r->intNo], 0x04);

        if (r->errCode) { 
            print(": 0x", 0x04);
            printHex(r->errCode, 0x04);
        }

        cli();
        hlt();
    } 
}


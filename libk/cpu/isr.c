#include <syscalls/syscalls.h>
#include <drivers/serial.h>
#include <cpu/system.h>
#include <cpu/idt.h>
#include <cpu/isr.h>

const char* format = "\x1b[31m[kernel] %s!\n\x1b[0m";
const char* exceptions[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
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
    "Reserved"
};

void remapPic() {
    outb(0x20, 0x11);
    outb(0xa0, 0x11);
    outb(0x21, 0x20);
    outb(0xa1, 0x28);
    outb(0x21, 0x04);
    outb(0xa1, 0x02);
    outb(0x21, 0x01);
    outb(0xa1, 0x01);
    outb(0x21, 0x00);
    outb(0xa1, 0x00);
}

HandlerPtr irqHandlers[16] = { 0 }; // IRQ 0-15

void setupIsrs() {
    setupIdt();
    remapPic(); // Remap IRQs 0-15 -> 32-48

    for (int i = 0; i < 48; i++) {
        idtSetGate(i, asm_isrRedirectTable[i], TRAP_GATE_FLAGS);
    }

    // System call
    idtSetGate(0x80, isr128, USR_INT_GATE_FLAGS);

    __asm__ volatile ("sti");
}

void registerIrq(uint8_t id, HandlerPtr handler) {
    irqHandlers[id] = handler;
}

void sendPicEOI(uint8_t irq) {
    if (irq >= 8) {
        outb(0xA0, 0x20);    // PIC2 Command
    }
    outb(0x20, 0x20);   // PIC1 Command
}

int handleInterrupt(intFrame_t iframe) {
    if (iframe.interrupt < 32) { 
        // ISRs 0-31 - Exceptions
        debugf(format, exceptions[iframe.interrupt]);
        panic();
    } else if (iframe.interrupt >= 32 && iframe.interrupt < 48) {
        // ISRs 32-47 - Hardware interrupts
        uint8_t irq = iframe.interrupt - 32;
        if (irqHandlers[irq]) {
            irqHandlers[irq](&iframe);
            sendPicEOI(irq);
        }
    } else if (iframe.interrupt == 128) { 
        // ISR 128 - System call
        return syscallHandler(&iframe);
    }

    return 0;
}


#include <syscall.h>
#include <kernel.h>
#include <string.h>
#include <system.h>
#include <idt.h>
#include <isr.h>

isr_handlerPtr irqHandlers[16];

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

// I have no clue how this works or what it does, but it doesn't work without it
void isr_remapPIC(void) {
	outb(0x20, 0x11); iowait();
	outb(0xa0, 0x11); iowait();
	outb(0x21, 0x20); iowait();
	outb(0xa1, 0x28); iowait();
	outb(0x21, 0x04); iowait();
	outb(0xa1, 0x02); iowait();
	outb(0x21, 0x01); iowait();
	outb(0xa1, 0x01); iowait();
	outb(0x21, 0x00); iowait();
	outb(0xa1, 0x00); iowait();
}

int isr_init(void) {
	idt_init();
	isr_remapPIC();

	for (int i = 0; i < 48; i++) {
		// First 32 IRSs are exceptions (traps) and the next 16 are hardware interrupts
		idt_setGate(i, isr_redirectTable[i], (i < 32) ? IDT_FLAG_TRAP : IDT_FLAG_INT);
	}

	// System call (should be callable from userspace)
	idt_setGate(0x80, (void*)(uint32_t)isr128, IDT_FLAG_USER);

	// Let interrupts interrupt
	__asm__ volatile ("sti");

	debugf("[isr] ISR initialized.\n");
	return 0;
}

void isr_registerIRQ(uint8_t i, isr_handlerPtr handler) {
	irqHandlers[i] = handler;
}

void isr_sendEOI(uint8_t irq) {
	if (irq >= 8) {
		outb(0xa0, 0x20);
	}
	outb(0x20, 0x20);
}

int isr_handleInterrupt(struct isr_intFrame iframe) {
	if (iframe.interrupt < 32) { 
		// ISRs 0-31 - Exceptions
		panic(exceptions[iframe.interrupt]);
	} else if (iframe.interrupt >= 32 && iframe.interrupt < 48) {
		// ISRs 32-47 - Hardware interrupts
		uint8_t irq = iframe.interrupt - 32;
		if (irqHandlers[irq]) {
			irqHandlers[irq](&iframe);
			isr_sendEOI(irq);
		}
	} else if (iframe.interrupt == 128) { 
		// ISR 128 - System call
		return syscallHandler(&iframe);
	}

	return 0;
}


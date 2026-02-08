#include <kernel.h>
#include <serial.h>
#include <string.h>
#include <system.h>
#include <idt.h>
#include <isr.h>

isr_handler_ptr irq_handlers[ISR_COUNT];

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
	"SIMD Floating-Point",
	"Virtualization",
	"Control Protection",
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

void pic_disable(void) {
	outb(0x21, 0xFF); iowait();
	outb(0xA1, 0xFF); iowait();
}

void pic_remap(void) {
	outb(0x20, 0x11); iowait();
	outb(0xA0, 0x11); iowait();
	outb(0x21, 0x20); iowait();
	outb(0xA1, 0x28); iowait();
	outb(0x21, 0x04); iowait();
	outb(0xA1, 0x02); iowait();
	outb(0x21, 0x01); iowait();
	outb(0xA1, 0x01); iowait();
	outb(0x21, 0x00); iowait();
	outb(0xA1, 0x00); iowait();
	// pic_disable(); For APIC
}

int isr_init(void) {
	memset(irq_handlers, 0, sizeof(irq_handlers));

	if (idt_init() != 0) {
		debugf("[isr] Could not load IDT.");
		return 1;
	}

	for (int i = 0; i < 48; i++) {
		// First 32 IRSs are exceptions (traps) and the next 16 are hardware interrupts
		idt_set_gate(i, isr_redirect_table[i], IDT_FLAG_INT);
	}
	idt_set_gate(0x03, isr_redirect_table[3], IDT_FLAG_USER); // Allow breakpoints
	idt_set_gate(0x80, isr128, IDT_FLAG_USER); // Syscall
						   //
	pic_remap();
	// TODO: apic_init();

	sti();
	debugf("[isr] ISR initialized.\n");
	return 0;
}

void isr_register(uint8_t i, isr_handler_ptr handler) {
	if (i >= ISR_COUNT) return; // secutiry B)
	irq_handlers[i] = handler;
}

void isr_send_eoi(uint8_t irq) {
	if (irq >= 8) outb(0xA0, 0x20);
	outb(0x20, 0x20);
}

// TODO: Write fault handlers and make this more robust 
void isr_handle_interrupt(size_t rsp) {
	struct isr_int_frame* iframe = (void*)rsp;

	if (iframe->interrupt < 32) { 
		if (iframe->error) debugf("[kernel] Error code: 0x%08x\n", iframe->error);
		panic(exceptions[iframe->interrupt]);
	} else if (iframe->interrupt >= 32 && iframe->interrupt < 48) {
		// ISRs 32-47 - Hardware interrupts
		uint8_t irq = iframe->interrupt - 32;
		isr_send_eoi(irq);
		if (irq_handlers[irq]) irq_handlers[irq](iframe);
	} else if (iframe->interrupt == 128) { 
		// ISR 128 - System call
		debugf("SYSCALL!\n");
	}
}


#include <kernel.h>
#include <string.h>
#include <system.h>
#include <errno.h>
#include <idt.h>
#include <isr.h>

isr_handler_ptr exception_handlers[EXCEPTION_COUNT];
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
	memset(exception_handlers, 0, sizeof(exception_handlers));
	memset(irq_handlers, 0, sizeof(irq_handlers));

	idt_init();

	for (int i = 0; i < 48; i++) {
		// First 32 IRSs are exceptions (traps) and the next 16 are hardware interrupts
		idt_set_gate(i, isr_redirect_table[i], i < 32 ? IDT_FLAG_TRAP : IDT_FLAG_INT, 0);
	}

	idt_set_gate(0x02, isr_redirect_table[2], IDT_FLAG_TRAP, 2); // Use IST2 for NMIs
	idt_set_gate(0x03, isr_redirect_table[3], IDT_FLAG_USER, 0); // Allow breakpoints
	idt_set_gate(0x08, isr_redirect_table[8], IDT_FLAG_TRAP, 1); // Use IST1 for Double Faults
	idt_set_gate(0x80, isr128, IDT_FLAG_USER, 0); // Syscall
	
	pic_remap();
	// TODO: apic_init();

	sti();
	return SUCCESS;
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
		if (exception_handlers[iframe->interrupt]) {
			exception_handlers[iframe->interrupt](iframe);
			return;
		}

		panic(iframe, "Unhandled %s exception, err 0x%lx", exceptions[iframe->interrupt], iframe->error);
	} else if (iframe->interrupt >= 32 && iframe->interrupt < 48) {
		uint8_t irq = iframe->interrupt - 32;

		if (irq_handlers[irq]) irq_handlers[irq](iframe);
		else pr_warn("Unhandled IRQ #%d\n", irq);

		isr_send_eoi(irq);
	} else if (iframe->interrupt == 0x80) { 
		// ISR 128 - System call
		pr_notice("SYSCALL!\n");
	} else {
		pr_warn("Unknown interrupt vector: %lu", iframe->interrupt);
	}
}


#include <kernel.h>
#include <string.h>
#include <gdt.h>
#include <idt.h>

__align(16) // performace :fire:
static struct idt_entry idt[IDT_ENTRIES];
static struct idt_pointer idtp;

void idt_setGate(uint8_t i, void* handler, uint8_t flags) {
	uint32_t offset = (uint32_t)handler;
	struct idt_entry* entry = &idt[i];
	
	entry->offsetLow = (uint16_t)offset;
	entry->selector = GDT_KERNEL_CODE;
	entry->flags = flags;
	entry->offsetHigh = (uint16_t)(offset >> 16);
}

int idt_init(void) {
	memset(&idt, 0, sizeof(idt));
	idtp.limit = sizeof(idt);
	idtp.base = (uint32_t)&idt;

	// Load IDT instruction
	__asm__ volatile ("lidt %0" : : "m"(idtp));

	debugf("[idt] IDT initalized.\n");
	return 0;
}


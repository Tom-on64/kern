#include <kernel.h>
#include <string.h>
#include <gdt.h>
#include <idt.h>

__align(16) // performace 
static struct idt_entry idt[IDT_ENTRIES];
static struct idt_pointer idtp;

void idt_set_gate(uint8_t i, void* handler, uint8_t attrib) {
	size_t addr = (size_t)handler;
	idt[i].offset_low = (uint16_t)addr;
	idt[i].selector = GDT_KERNEL_CODE;
	idt[i].ist = 0;
	idt[i].__reserved = 0;
	idt[i].attrib = attrib;
	idt[i].offset_mid = (uint16_t)(addr >> 16);
	idt[i].offset_high = (uint32_t)(addr >> 32);
}

int idt_init(void) {
	idtp.size = sizeof(idt) - 1;
	idtp.offset = (size_t)&idt;

	// Load IDT instruction
	__asm__ volatile ("lidt %0" : : "m"(idtp));

	debugf("[idt] IDT initalized.\n");
	return 0;
}


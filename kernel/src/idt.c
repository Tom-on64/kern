#include <kernel.h>
#include <string.h>
#include <system.h>
#include <errno.h>
#include <gdt.h>
#include <idt.h>

__align(16) // performace 
static struct idt_entry idt[IDT_ENTRIES];
static struct idt_pointer idtp;

void idt_set_gate(uint8_t i, void* handler, uint8_t attrib, uint8_t ist) {
	size_t addr = (size_t)handler;
	idt[i].offset_low = (uint16_t)addr;
	idt[i].selector = GDT_KERNEL_CODE;
	idt[i].ist = ist & 7;
	idt[i].attrib = attrib;
	idt[i].offset_mid = (uint16_t)(addr >> 16);
	idt[i].offset_high = (uint32_t)(addr >> 32);
	idt[i].__reserved = 0;
}

int idt_init(void) {
	memset(idt, 0, sizeof(idt));

	idtp.size = sizeof(idt) - 1;
	idtp.offset = (uintptr_t)&idt;

	// Load IDT instruction
	cli();
	__asm__ volatile ("lidt %0" : : "m"(idtp));

	return SUCCESS;
}


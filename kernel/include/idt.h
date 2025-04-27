#ifndef _IDT_H
#define _IDT_H

#include <kernel.h>

#define IDT_ENTRIES	256
#define IDT_FLAG_TRAP	0x8f	// P = 1, Priv = 00, S = 0, Type = 1111 (32-bit trap)
#define IDT_FLAG_INT	0x8e	// P = 1, Priv = 00, S = 0, Type = 1110 (32-bit int)
#define IDT_FLAG_USER 	0xee	// P = 1, Priv = 11, S = 0, Type = 1110 (32-bit int)

struct idt_entry {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t  __unused;
	uint8_t  flags;
	uint16_t offset_high;
} __packed;

struct idt_pointer {
	uint16_t limit;
	uint32_t base;
} __packed;

int idt_init(void);
void idt_set_gate(uint8_t i, void* handler, uint8_t flags);

#endif

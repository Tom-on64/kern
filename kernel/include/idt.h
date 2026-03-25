#ifndef _IDT_H
#define _IDT_H

#include <kernel.h>

#define IDT_ENTRIES	256
#define IDT_FLAG_TRAP	0x8F	// P = 1, Priv = 00, S = 0, Type = 1111 (32-bit trap)
#define IDT_FLAG_INT	0x8E	// P = 1, Priv = 00, S = 0, Type = 1110 (32-bit int)
#define IDT_FLAG_USER 	0xEE	// P = 1, Priv = 11, S = 0, Type = 1110 (32-bit int)

/*
 * Attribute field explanation
 * 0..3	Gate type
 * 4	Zero (unused)
 * 5..6	DPL (privilege)
 * 7	Present
 */

struct idt_entry {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t  ist;	// Only 3 low bits, rest is reserved
	uint8_t  attrib;
	uint16_t offset_mid;
	uint32_t offset_high;
	uint32_t __reserved;
} __packed;

struct idt_pointer {
	uint16_t size;
	uint64_t offset;
} __packed;

int idt_init(void);
void idt_set_gate(uint8_t i, void* handler, uint8_t flags, uint8_t ist);

#endif

#ifndef _GDT_H
#define _GDT_H

#include <kernel.h>

// Privilege levels
#define DPL_KERNEL	0
#define DPL_USER	3

/*
 * Segment selector explanation (why is kcode 0x08?)
 *
 * Each segment selector must be 16 bits. These bits
 * have the following layout: (example is 0x08)
 *   0000000000001 0 00 (lsb)
 *              |  | '- Privilege level
 *              |  '--- Table indicator
 *              '------ Index in table
 *
 * Bits 0-1 are the privilege level. x86(_64) has four, 
 * where 00 is the highest and 11 is the lowest. Only
 * these two are widely used. 00 being kernel space &
 * 11 being userspace. Bit 3 is the table indicator.
 * 0 means the GDT and 1 means the LDT. This will most
 * likely be 0. Bits 4-15 are the index. This is just
 * which descriptor in the table is used.
 *
 * SO, for the kernel code segment:
 * Privilege is 00 for kernel space, table indicator
 * s 0 for the GDT and the index is one, because it
 * is the second entry in our GDT (after null desc.)
 */

/*
 * Entries:
 * 0 Null desc.
 * 1 Kernel code desc.
 * 2 Kernel data desc.
 * 3 User data desc.
 * 4 User code desc.
 * 5 Task State Segment
 */
#define GDT_ENTRIES	7
#define GDT_NULL	0x00
#define GDT_KERNEL_CODE	0x08
#define GDT_KERNEL_DATA	0x10
#define GDT_USER_DATA	0x18
#define GDT_USER_CODE	0x20
#define GDT_TSS		0x28	

/*
 * GDT descriptor/entry explanation
 *
 * 0-15	 low 16 bits of limit
 * 15-31 low 16 bits of base
 * 32-39 mid 8 bits of base
 * 40-47 access byte
 * 	40 Accessed
 * 	41 Read/Write
 * 	42 Direction/Conforming
 * 	43 Executable
 * 	44 Descriptor type
 * 	45-46 DPL
 * 	47 Present
 * 48-51 high 4 bits of limit
 * 52-55 flags
 * 	52 Reserved
 * 	53 Long-mode
 * 	54 Size (0 - 16-bit, 1 - 32-bit)
 * 	55 Granularity (0 - limit in bytes, 1 - limit in pages)
 * 56-63 high 8 bits of base
 */
struct gdt_entry {
	uint16_t limit;
	uint16_t base_low;
	uint8_t	 base_mid;
	uint8_t  access;
	uint8_t  flags;		// low 4 bits are 4 msbs of limit, high 4 bits are flags
	uint8_t  base_high;
} __packed;

struct gdt_pointer {
	uint16_t limit;
	uint64_t base;
} __packed;

struct tss_entry {
	uint16_t length;
	uint16_t base_low;
	uint8_t	 base_mid;
	uint8_t  access;
	uint8_t  flags;
	uint8_t  base_high;
	uint32_t base_upper32;
	uint32_t __reserved;
} __packed;

struct tss_pointer {
	uint32_t __reserved0;
	uint64_t rsp0;
	uint64_t rsp1;
	uint64_t rsp2;
	uint64_t __reserved1;
	uint64_t ist1;
	uint64_t ist2;
	uint64_t ist3;
	uint64_t ist4;
	uint64_t ist5;
	uint64_t ist6;
	uint64_t ist7;
	uint64_t __reserved2;
	uint16_t __reserved3;
	uint16_t iopb;
} __packed;

struct gdt_entries {
	// GDT_ENTRIES - 2 to leave space for TSS
	struct gdt_entry entries[GDT_ENTRIES - 2];
	struct tss_entry tss;
} __packed;

int gdt_init(void);

#endif

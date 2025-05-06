#ifndef _GDT_H
#define _GDT_H

#include <kernel.h>

// Privilege levels
#define DPL_KERNEL	0
#define DPL_USER	3

/*
 * Segment selector explanation (why kcode is 0x08?)
 *
 * Each segment selector must be 16 bits. These bits
 * have the following layout: (example is 0x08)
 *   0000000000001 0 00 (lsb)
 *              |  | '- Privilege level
 *              |  '--- Table indicator
 *              '------ Index in table
 *
 * Bits 0-1 are the privilege level. x86 has four, 
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
#define SEGMENT_SEL(_idx, _tab, _dpl) (\
	(((_dpl)     ) & 3) |		\
	(((_tab) << 2) & 4) |		\
	(((_idx) << 3)    ) 		\
	)

/*
 * Entries:
 * 0 Null desc.
 * 1 Kernel code desc.
 * 2 Kernel data desc.
 * 3 User code desc.
 * 4 User data desc.
 * 5 Task State Segment
 */
#define GDT_ENTRIES	6
#define GDT_NULL	SEGMENT_SEL(0, 0, 0)
#define GDT_KERNEL_CODE	SEGMENT_SEL(1, 0, DPL_KERNEL)
#define GDT_KERNEL_DATA	SEGMENT_SEL(2, 0, DPL_KERNEL)
#define GDT_USER_CODE	SEGMENT_SEL(3, 0, DPL_USER)
#define GDT_USER_DATA	SEGMENT_SEL(4, 0, DPL_USER)
#define GDT_TSS		SEGMENT_SEL(5, 0, DPL_KERNEL)

#define GDT_DESC(_lim, _base, _access, _flags) (struct gdt_entry) { \
	.base_low = (_base) & 0xFFFF, \
	.base_mid = ((_base) >> 16) & 0xFF, \
	.base_high = ((_base) >> 24) & 0xFF, \
	.limit = (_lim) & 0xFFFF, \
	.flags = ((_flags) & 0xF0) | (((_lim) >> 16) & 0x0F), \
	.access = (_access) \
}

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
	uint32_t base;
} __packed;

struct tss_pointer {
	uint16_t prev_task, __reserved0;
	uint32_t esp0;
	uint16_t ss0, __reserved1;
	uint32_t esp1;
	uint16_t ss1, __reserved2;
	uint32_t esp2;
	uint16_t ss2, __reserved3;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
	uint16_t es, __reserved4;
	uint16_t cs, __reserved5;
	uint16_t ss, __reserved6;
	uint16_t ds, __reserved7;
	uint16_t fs, __reserved8;
	uint16_t gs, __reserved9;
	uint16_t ldtr;
	uint32_t __reserved10;
	uint16_t iopb;
	uint32_t ssp;
} __packed;

void tss_esp0(uint32_t esp0);
int gdt_init(void);

#endif

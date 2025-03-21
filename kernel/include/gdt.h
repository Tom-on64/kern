#ifndef _GDT_H
#define _GDT_H

#include <kernel.h>

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
#define SEGMENT_SEL(_priv, _tab, _i)\
	(uint16_t)(((_i) << 3) | ((_tab) << 2) | (_priv))

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
#define GDT_KERNEL_CODE	SEGMENT_SEL(0, 0, 1)
#define GDT_KERNEL_DATA	SEGMENT_SEL(0, 0, 2)
#define GDT_USER_CODE	SEGMENT_SEL(3, 0, 3)
#define GDT_USER_DATA	SEGMENT_SEL(3, 0, 4)
#define GDT_TSS		SEGMENT_SEL(0, 0, 5)

#define GDT_DESC(_lim, _base, _acc, _flag) (struct gdt_entry){\
	.limit 		= (_lim) & 0xffff,\
	.baseLow	= (uint16_t)(_base),\
	.baseMid	= (uint8_t)((_base) >> 16),\
	.access		= (_acc),\
	.flags		= (_flag << 4) | (((_lim) >> 16) & 0x0f),\
	.baseHigh	= (uint8_t)((_base) >> 24),\
}

struct gdt_entry {
	uint16_t limit;
	uint16_t baseLow;
	uint8_t	 baseMid;
	uint8_t  access;
	uint8_t  flags;		// low 4 bits are 4 msbs of limit, high 4 bits are flags
	uint8_t  baseHigh;
} __packed;

struct gdt_pointer {
	uint16_t limit;
	uint32_t base;
} __packed;

struct tss_pointer {
	uint16_t link;
	uint16_t __reserved0;
	uint32_t esp0;
	uint16_t ss0;
	uint16_t __reserved1;
	uint32_t esp1;
	uint16_t ss1;
	uint16_t __reserved2;
	uint32_t esp2;
	uint16_t ss2;
	uint16_t __reserved3;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint16_t es;
	uint16_t __reserved4;
	uint16_t cs;
	uint16_t __reserved5;
	uint16_t ss;
	uint16_t __reserved6;
	uint16_t ds;
	uint16_t __reserved7;
	uint16_t fs;
	uint16_t __reserved8;
	uint16_t gs;
	uint16_t __reserved9;
	uint16_t ldtr;
	uint32_t __reserved10;
	uint16_t iopb;
	uint32_t ssp;
} __packed;

int gdt_init(void);

#endif

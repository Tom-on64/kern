#include <kernel.h>
#include <string.h>
#include <gdt.h>

static struct gdt_entries gdt;
static struct gdt_pointer gdtp;
static struct tss_pointer tss;
struct tss_pointer* tssp = &tss;

void gdt_loadTss(struct tss_pointer* ptr) {
	size_t addr = (size_t)ptr;

	gdt.tss.baseLow = (uint16_t)addr;
	gdt.tss.baseMid = (uint8_t)(addr >> 16);
	gdt.tss.flags1 = 0x89;
	gdt.tss.flags2 = 0;
	gdt.tss.baseHigh = (uint8_t)(addr >> 24);
	
	__asm__ volatile ("ltr %0" : : "rm"(SEGMENT_SEL(0, 0, 7)));
}

void gdt_reload(void) {
	__asm__ volatile (
		"cli\n"
        	"lgdt (%0)\n"
		"ljmp $0x08, $1f\n"
        	"1:\n"
        	"movw $0x10, %%ax\n"
        	"movw %%ax, %%ds\n"
        	"movw %%ax, %%es\n"
        	"movw %%ax, %%fs\n"
        	"movw %%ax, %%gs\n"
        	"movw %%ax, %%ss\n"
        	:
		: "m" (gdtp)
        	: "memory", "rax"
	);
}

int gdt_init(void) {
	// Null descriptor (0)
	gdt.descriptors[0].limit = 0;
	gdt.descriptors[0].baseLow = 0;
	gdt.descriptors[0].baseMid = 0;
	gdt.descriptors[0].access = 0;
	gdt.descriptors[0].granulatiry = 0;
	gdt.descriptors[0].baseHigh = 0;

	// Kernel code (8)
	gdt.descriptors[1].limit = 0xffff;
	gdt.descriptors[1].baseLow = 0;
	gdt.descriptors[1].baseMid = 0;
	gdt.descriptors[1].access = 0x9a;
	gdt.descriptors[1].granulatiry = 0xcf;
	gdt.descriptors[1].baseHigh = 0;

	// Kernel data (16)
	gdt.descriptors[2].limit = 0xffff;
	gdt.descriptors[2].baseLow = 0;
	gdt.descriptors[2].baseMid = 0;
	gdt.descriptors[2].access = 0x90;
	gdt.descriptors[2].granulatiry = 0xcf;
	gdt.descriptors[2].baseHigh = 0;

	// SYSENTER/SYSEXIT
	gdt.descriptors[3] = (struct gdt_entry){ 0 }; // (24)
	gdt.descriptors[4] = (struct gdt_entry){ 0 }; // (32)

	// User code (40)
	gdt.descriptors[5].limit = 0;
	gdt.descriptors[5].baseLow = 0;
	gdt.descriptors[5].baseMid = 0;
	gdt.descriptors[5].access = 0xfa;
	gdt.descriptors[5].granulatiry = 0x20;
	gdt.descriptors[5].baseHigh = 0;

	// User data (48)
	gdt.descriptors[6].limit = 0;
	gdt.descriptors[6].baseLow = 0;
	gdt.descriptors[6].baseMid = 0;
	gdt.descriptors[6].access = 0xf2;
	gdt.descriptors[6].granulatiry = 0;
	gdt.descriptors[6].baseHigh = 0;

	// TSS (56)
	gdt.tss.length = 104;
	gdt.tss.baseLow = 0;
	gdt.tss.baseMid = 0;
	gdt.tss.flags1 = 0x89;
	gdt.tss.flags2 = 0;
	gdt.tss.baseHigh = 0;

	gdtp.limit = sizeof(struct gdt_entries) - 1;
	gdtp.base = (uint32_t)&gdt;

	gdt_reload();

	memset(tssp, 0, sizeof(struct tss_pointer));
	gdt_loadTss(tssp);
	return 0;
}


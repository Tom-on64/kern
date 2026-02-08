#include <kernel.h>
#include <string.h>
#include <gdt.h>

static struct gdt_entries gdt;
static struct gdt_pointer gdtp;
static struct tss_pointer tss;

void gdt_reload(void) {
	__asm__ volatile (
		"lgdt %0\n"
		"pushq %[kcs]\n"
		"lea 1f(%%rip), %%rax\n"
		"pushq %%rax\n"
		"lretq\n"
		"1:\n"
		"mov %[kds], %%ax\n"
		"mov %%ax, %%ds\n"
		"mov %%ax, %%es\n"
		"mov %%ax, %%fs\n"
		"mov %%ax, %%gs\n"
		"mov %%ax, %%ss\n"
		:
		: "m"(gdtp),
		  [kcs] "i"(GDT_KERNEL_CODE),
		  [kds] "i"(GDT_KERNEL_DATA)
		: "rax", "memory"
	);
}

void tss_load(struct tss_pointer* tss) {
	size_t addr = (size_t)tss;

	gdt.tss.length = 104;
	gdt.tss.base_low = (uint16_t)addr;
	gdt.tss.base_mid = (uint8_t)(addr >> 16);
	gdt.tss.access = 0x89;
	gdt.tss.flags = 0;
	gdt.tss.base_high = (uint8_t)(addr >> 24);
	gdt.tss.base_upper32 = (uint32_t)(addr >> 32);
	gdt.tss.__reserved = 0;

	__asm__ volatile ("ltr %0" : : "rm"(GDT_TSS) : "memory");
}

int gdt_init(void) {
	// Null descriptor
	gdt.entries[0].limit = 0;
	gdt.entries[0].base_low = 0;
	gdt.entries[0].base_mid = 0;
	gdt.entries[0].access = 0;
	gdt.entries[0].flags = 0;
	gdt.entries[0].base_high = 0;

	// Kernel code
	gdt.entries[1].limit = 0xFFFF;
	gdt.entries[1].base_low = 0;
	gdt.entries[1].base_mid = 0;
	gdt.entries[1].access = 0x9A;
	gdt.entries[1].flags = 0xAF;
	gdt.entries[1].base_high = 0;

	// Kernel data
	gdt.entries[2].limit = 0xFFFF;
	gdt.entries[2].base_low = 0;
	gdt.entries[2].base_mid = 0;
	gdt.entries[2].access = 0x92;
	gdt.entries[2].flags = 0xCF;
	gdt.entries[2].base_high = 0;

	// User data
	gdt.entries[3].limit = 0xFFFF;
	gdt.entries[3].base_low = 0;
	gdt.entries[3].base_mid = 0;
	gdt.entries[3].access = 0xF2;
	gdt.entries[3].flags = 0xCF;
	gdt.entries[3].base_high = 0;

	// User code
	gdt.entries[4].limit = 0xFFFF;
	gdt.entries[4].base_low = 0;
	gdt.entries[4].base_mid = 0;
	gdt.entries[4].access = 0xFA;
	gdt.entries[4].flags = 0xAF;
	gdt.entries[4].base_high = 0;

	// Task State Segment (TSS)
	gdt.tss.length = 104;
	gdt.tss.base_low = 0;
	gdt.tss.base_mid = 0;
	gdt.tss.access = 0x89; //0b10001001;
	gdt.tss.flags = 0;
	gdt.tss.base_high = 0;
	gdt.tss.base_upper32 = 0;
	gdt.tss.__reserved = 0;

	// GDT Pointer
	gdtp.limit = sizeof(gdt) - 1;
	gdtp.base = (uint64_t)&gdt;

	gdt_reload();

	memset(&tss, 0, sizeof(tss));
	tss_load(&tss);

	debugf("[gdt] GDT loaded.\n");

	return 0;
}


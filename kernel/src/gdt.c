#include <kernel.h>
#include <string.h>
#include <gdt.h>

static struct gdt_entry gdt[GDT_ENTRIES];
static struct gdt_pointer gdtp;
static struct tss_pointer tss;

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

void tss_reload(void) {
	__asm__ volatile ("ltr %0" : : "rm"(GDT_TSS));
	gdt[5].access |= 0x02;
}

void tss_esp0(uint32_t esp0) { tss.esp0 = esp0; }

int gdt_init(void) {
	// Null descriptor (0)
	gdt[0] = GDT_DESC(0, 0, 0, 0);

	// Kernel code (8)
	gdt[1] = GDT_DESC(0xfffff, 0, 0x9a, 0xc);

	// Kernel data (16)
	gdt[2] = GDT_DESC(0xfffff, 0, 0x92, 0xc);

	// User code (24)
	gdt[3] = GDT_DESC(0xfffff, 0, 0xfa, 0xc);

	// User data (32)
	gdt[4] = GDT_DESC(0xfffff, 0, 0xf2, 0xc);

	// TSS (40)
	memset(&tss, 0, sizeof(tss));
	gdt[5] = GDT_DESC(sizeof(tss) - 1, (uint32_t)&tss, 0x89, 0x40);
	tss.iopb = sizeof(struct tss_pointer);
	tss.esp0 = __kernel_stack_top;
	tss.ss0 = GDT_KERNEL_DATA;

	// GDT Pointer
	gdtp.limit = sizeof(gdt) - 1;
	gdtp.base = (uint32_t)&gdt;

	gdt_reload();
	tss_reload();

	return 0;
}


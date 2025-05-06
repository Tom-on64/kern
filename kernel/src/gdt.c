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
        	"movw $0x10, %%ax\n"
        	"movw %%ax, %%ds\n"
        	"movw %%ax, %%es\n"
        	"movw %%ax, %%fs\n"
        	"movw %%ax, %%gs\n"
        	"movw %%ax, %%ss\n"
		"ljmp $0x08, $1f\n"
        	"1:"
        	:
		: "m" (gdtp)
        	: "memory", "rax"
	);
}

void tss_reload(void) {
	__asm__ volatile ("ltr %0" : : "rm"(GDT_TSS));
}

void tss_esp0(uint32_t esp0) { tss.esp0 = esp0; }

int gdt_init(void) {
	// GDT Pointer
	gdtp.limit = sizeof(gdt) - 1;
	gdtp.base = (uint32_t)&gdt;

	// Setup TSS
	memset(&tss, 0, sizeof(tss));
	tss.ss0 = GDT_KERNEL_DATA;
	tss.iopb = sizeof(struct tss_pointer);

	// Load descriptors
	gdt[0] = GDT_DESC(0, 0, 0, 0);			// Null descriptor
	gdt[1] = GDT_DESC(0xFFFFFFFF, 0, 0x9A, 0xC0);	// Kernel code
	gdt[2] = GDT_DESC(0xFFFFFFFF, 0, 0x92, 0xC0);	// Kernel data
	gdt[3] = GDT_DESC(0xFFFFFFFF, 0, 0xFA, 0xC0);	// User code
	gdt[4] = GDT_DESC(0xFFFFFFFF, 0, 0xF2, 0xC0);	// User data
	gdt[5] = GDT_DESC(sizeof(tss) - 1, (uint32_t)&tss, 0x89, 0);

	gdt_reload();
	tss_reload();

	debugf("[gdt] GDT loaded.\n");

	return 0;
}


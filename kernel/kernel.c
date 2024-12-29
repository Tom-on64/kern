#include "stdint.h"
#include "multiboot.h"

/* Constants */
#define COLS	80
#define ROWS	24
#define ATTR	0x0f
#define VID_MEM	0xb8000

/* Variables */
static int xpos;
static int ypos;
static uint8_t* video;

/* Forward Declarations */
void cls(void);
void itoa(char* buf, int base, int d);
void putc(int c);
void printf(const char* fmt, ...);

/* Entry Point */
void kmain(void) {
	uint32_t magic, addr;
	__asm__ volatile ("movl %%eax, %0; movl %%ebx, %1" : "=m"(magic), "=m"(addr));
	struct multiboot_info* mbi;
  	video = (uint8_t*)VID_MEM;

	// Clear Screen
	cls();

	// Didn't boot with Multiboot
	if (magic != BOOT_MAGIC) {
		printf("Invalid Magic Number: 0x%x\n", magic);
		while (1);
	}
	
	mbi = (struct multiboot_info*)addr;

	printf("flags: 0x%x\n", mbi->flags);

	if (mbi->flags & MBI_MEMORY) {
		printf("mem_lower: %ukB, mem_upper: %ukB\n", 
			mbi->mem_lower, mbi->mem_upper);
	}

	if (mbi->flags & MBI_BOOT_DEV) {
		printf("boot_device: 0x%x\n", mbi->boot_device);
	}

	if (mbi->flags & MBI_CMDLINE) {
		printf("cmdline: %s\n", mbi->cmdline);
	}

	if (mbi->flags & MBI_MODS) {
		printf("mods_count: %d, mods_addr: 0x%x\n",
			mbi->mods_count, mbi->mods_addr);

		struct multiboot_module* mod = 
			(struct multiboot_module*)mbi->mods_addr;

		for (size_t i = 0; i < mbi->mods_count; i++) {
			printf("  start 0x%x, end: 0x%x, cmdline: %s\n",
				mod->start, mod->end, mod->cmdline);
			mod++;
		}
	}

	if (mbi->flags & MBI_SYMBOLS &&
	    mbi->flags & MBI_ELF_HEAD) {
		printf("Flag SYMBOLS and ELF HEADER are both set!\n");
		while (1);
	}

	if (mbi->flags & MBI_MEMMAP) {
		printf("mmap_addr: 0x%x, mmap_length: 0x%x\n",
			mbi->mmap_addr, mbi->mmap_length);

		struct multiboot_memmap* mmap = 
			(struct multiboot_memmap*)mbi->mmap_addr;

		while ((uint32_t)mmap < mbi->mmap_addr + mbi->mmap_length) {
			printf(	"  size: 0x%x, base: 0x%08x, "
				"length: 0x%08x, type: 0x%x\n",
				mmap->size, mmap->base, mmap->len, mmap->type);

			mmap = (struct multiboot_memmap*)
				((uint32_t)mmap + mmap->size + (sizeof(mmap->size)));
		}
	}

	while (1);
}

void cls(void) {
	for (int i = 0; i < COLS * ROWS * 2; i++) video[i] = 0;
	xpos = 0;
	ypos = 0;
}

void itoa(char* buf, int base, int d) {
	char* p = buf;
	char* p1;
	char* p2;
	uint32_t ud = d;
	int divisor = 10;

	if (base == 'd' && d < 0) {
		*p++ = '-';
		buf++;
		ud = -d;
	} else if (base == 'x') divisor = 16;

	do {
		int remainder = ud % divisor;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
	} while (ud /= divisor);

	*p = 0;

	p1 = buf;
	p2 = p - 1;
	while (p1 < p2) {
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

/* Put the character C on the screen. */
void putc(int c) {
	if (c == '\n' || c == '\r') {
	newline:
		xpos = 0;
		ypos++;
		if (ypos >= ROWS)
		ypos = 0;
		return;
	}

	video[(xpos + ypos * COLS) * 2] = c & 0xFF;
	video[(xpos + ypos * COLS) * 2 + 1] = ATTR;

	xpos++;
	if (xpos >= COLS)
	goto newline;
}

void printf(const char *format, ...) {
	char** arg = (char**)&format;
	int c;
	char buf[20];

	arg++;

	while ((c = *format++) != 0) {
		if (c != '%') { 
			putc (c);
			continue;
		}
		
		char *p, *p2;
		int pad0 = 0, pad = 0;
		
		c = *format++;
		
		if (c == '0') {
			pad0 = 1;
			c = *format++;
		}

		if (c >= '0' && c <= '9') {
			pad = c - '0';
			c = *format++;
		}

		switch (c) {
		case 'd':
		case 'u':
		case 'x':
			itoa(buf, c, *((int *) arg++));
			p = buf;
			goto string;
			break;
		case 's':
			p = *arg++;
			if (! p) p = "(null)";
		string:
			for (p2 = p; *p2; p2++);
			for (; p2 < p + pad; p2++) putc (pad0 ? '0' : ' ');
			while (*p) putc (*p++);
			break;
		default:
			putc(*((int *) arg++));
			break;
		}
	}
}


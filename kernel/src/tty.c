#include <kernel.h>
#include <paging.h>
#include <system.h>
#include <tty.h>

struct {
	size_t row;
	size_t col;
	uint8_t attr;
	uint8_t* buf;
	size_t updateWait;
} ctx;

int tty_init(void) {
	// Map VGA Text memory to KERNEL_FRAMEBUF
	size_t pagecount = dceil(VGA_WIDTH * VGA_HEIGHT * 2, PAGE_SIZE);
	for (size_t i = 0; i < pagecount; i++) {
		pag_mapPage(KERNEL_FRAMEBUF + i * PAGE_SIZE, VGA_MEMORY + i * PAGE_SIZE, 0);
	}

	ctx.row = 0;
	ctx.col = 0;
	ctx.attr = 0x07;
	ctx.buf = (uint8_t*)KERNEL_FRAMEBUF;
	tty_clear();

	return 0;
}

void tty_clear(void) {
	for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
		ctx.buf[i * 2] = ' ';
		ctx.buf[i * 2 + 1] = ctx.attr;
	}
}

void tty_setattr(uint8_t attr) {
	ctx.attr = attr;
}

void tty_enableCursor(uint8_t start, uint8_t end) {
	outb(0x3d4, 0x0a);
	outb(0x3d5, (inb(0x3d5) & 0xc0) | start);
	outb(0x3d4, 0x0b);
	outb(0x3d5, (inb(0x3d5) & 0xe0) | end);
}

void tty_disableCursor(void) {
	outb(0x3d4, 0x0a);
	outb(0x3d5, 0x20);
}

void tty_updateCursor(void) {
	if (ctx.updateWait > 0) {
		ctx.updateWait--;
		return;
	}

	uint16_t pos = ctx.row * VGA_WIDTH + ctx.col;
	outb(0x3d4, 0x0f);
	outb(0x3d5, (uint8_t)pos);
	outb(0x3d4, 0x0e);
	outb(0x3d5, (uint8_t)(pos >> 8));
}

// TODO:
void tty_scroll(void) {
	ctx.row = 0;
	ctx.col = 0;
	tty_clear();
}

void tty_putat(char c, size_t row, size_t col) {
	size_t i = row * VGA_WIDTH + col;
	ctx.buf[i * 2] = c;
	ctx.buf[i * 2 + 1] = ctx.attr;
}

void tty_putc(char c) {
	if (c == '\n') { 
		ctx.col = 0;
		ctx.row++;
		goto scroll_check;
	}

	tty_putat(c, ctx.row, ctx.col++);
	if (ctx.col >= VGA_WIDTH) {
		ctx.col = 0;
		ctx.row++;
	}

scroll_check:
	if (ctx.row >= VGA_HEIGHT) tty_scroll();
	tty_updateCursor();
}

void tty_write(char* buf, size_t len) {
	ctx.updateWait = len - 1;
	for (size_t i = 0; i < len; i++) tty_putc(buf[i]);
}

void tty_puts(char* s) {
	size_t len = 0;
	char* p = s;
	while (*p++ != '\0') len++;
	tty_write(s, len);
}


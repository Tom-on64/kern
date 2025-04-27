#include <kernel.h>
#include <paging.h>
#include <system.h>
#include <tty.h>

struct {
	size_t	 row;
	size_t	 col;
	uint8_t  attr;
	uint8_t* buf;
	/*
	 * This is for the tty_update_cursor() function. It makes sure we don't update
	 * the cursor if we don't need to, because it's slow due to I/O port writes.
	 */
	size_t	 update_wait;
} tty_ctx;

int tty_init(void) {
	// Map VGA Text memory to KERNEL_FRAMEBUF
	size_t page_count = dceil(VGA_WIDTH * VGA_HEIGHT * 2, PAGE_SIZE);
	for (size_t i = 0; i < page_count; i++) {
		pag_map(KERNEL_FRAMEBUF + i * PAGE_SIZE, VGA_MEMORY + i * PAGE_SIZE, 0);
	}

	tty_ctx.row = 0;
	tty_ctx.col = 0;
	tty_ctx.attr = 0x07;
	tty_ctx.buf = (uint8_t*)KERNEL_FRAMEBUF;
	tty_clear();

	return 0;
}

void tty_clear(void) {
	for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
		tty_ctx.buf[i * 2] = ' ';
		tty_ctx.buf[i * 2 + 1] = tty_ctx.attr;
	}
}

void tty_set_attr(uint8_t attr) {
	tty_ctx.attr = attr;
}

void tty_show_cursor(uint8_t start, uint8_t end) {
	outb(0x3d4, 0x0a);
	outb(0x3d5, (inb(0x3d5) & 0xc0) | start);
	outb(0x3d4, 0x0b);
	outb(0x3d5, (inb(0x3d5) & 0xe0) | end);
}

void tty_hide_cursor(void) {
	outb(0x3d4, 0x0a);
	outb(0x3d5, 0x20);
}

void tty_update_cursor(void) {
	if (tty_ctx.update_wait > 0) {
		tty_ctx.update_wait--;
		return;
	}

	uint16_t pos = tty_ctx.row * VGA_WIDTH + tty_ctx.col;
	outb(0x3d4, 0x0f);
	outb(0x3d5, (uint8_t)pos);
	outb(0x3d4, 0x0e);
	outb(0x3d5, (uint8_t)(pos >> 8));
}

// TODO:
void tty_scroll(void) {
	tty_ctx.row = 0;
	tty_ctx.col = 0;
	tty_clear();
}

void tty_putat(char c, size_t row, size_t col) {
	size_t i = row * VGA_WIDTH + col;
	tty_ctx.buf[i * 2] = c;
	tty_ctx.buf[i * 2 + 1] = tty_ctx.attr;
}

void tty_putc(char c) {
	if (c == '\n') { 
		tty_ctx.col = 0;
		tty_ctx.row++;
		goto scroll_check;
	}

	tty_putat(c, tty_ctx.row, tty_ctx.col++);
	if (tty_ctx.col >= VGA_WIDTH) {
		tty_ctx.col = 0;
		tty_ctx.row++;
	}

scroll_check:
	if (tty_ctx.row >= VGA_HEIGHT) tty_scroll();
	tty_update_cursor();
}

void tty_write(char* buf, size_t len) {
	tty_ctx.update_wait = len - 1;
	for (size_t i = 0; i < len; i++) tty_putc(buf[i]);
}

void tty_puts(char* s) {
	size_t len = 0;
	char* p = s;
	while (*p++ != '\0') len++;
	tty_write(s, len);
}


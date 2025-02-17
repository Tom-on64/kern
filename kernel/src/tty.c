#include <kernel.h>
#include <tty.h>

struct {
	size_t row;
	size_t col;
	uint8_t attr;
	uint8_t* buf;
} ctx;

void tty_init(void) {
	ctx.row = 0;
	ctx.col = 0;
	ctx.attr = 0x07;
	ctx.buf = (uint8_t*)VGA_MEMORY;
	tty_clear();
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
		if (ctx.row >= VGA_HEIGHT) tty_scroll();
		return;
	}

	tty_putat(c, ctx.row, ctx.col++);
	if (ctx.col >= VGA_WIDTH) {
		ctx.col = 0;
		ctx.row++;
	}

	if (ctx.row >= VGA_HEIGHT) tty_scroll();
}

void tty_puts(char* s) {
	while (*s != '\0') tty_putc(*s++);
}


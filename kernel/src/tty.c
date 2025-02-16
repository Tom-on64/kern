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
	ctx.buf = (u8*)VGA_MEMORY;

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			size_t i = (y * VGA_WIDTH + x) * 2;
			ctx.buf[i] = ' ';
			ctx.buf[i + 1] = ctx.attr;
		}
	}
}

int tty_write(char* buf, size_t len) {}

int tty_putc(char c) {}

int tty_puts(char* s) {}


#ifndef _TTY_H
#define _TTY_H

/* VGA Text mode */
#define VGA_ATTR(_fg, _bg)	((_fg) | ((_bg) << 4)
#define VGA_WIDTH	80
#define VGA_HEIGHT	25
#define VGA_MEMORY	0xb8000

void tty_init(void);
void tty_clear(void);
void tty_setattr(uint8_t attr);
void tty_putc(char c);
void tty_puts(char* s);

#endif

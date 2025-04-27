#ifndef _TTY_H
#define _TTY_H

/* VGA Text mode */
#define VGA_ATTR(_fg, _bg)	((_fg) | ((_bg) << 4)
#define VGA_WIDTH	80
#define VGA_HEIGHT	25
#define VGA_MEMORY	0xb8000

int  tty_init(void);
void tty_clear(void);
void tty_set_attr(uint8_t attr);
void tty_show_cursor(uint8_t start, uint8_t end);
void tty_hide_cursor(void);
void tty_write(char* buf, size_t len);
void tty_putc(char c);
void tty_puts(char* s);

#endif

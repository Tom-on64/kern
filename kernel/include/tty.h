#ifndef _TTY_H
#define _TTY_H

/* VGA Text mode */
#define VGA_WIDTH	80
#define VGA_HEIGHT	25
#define VGA_MEMORY	0xb8000

int tty_init(void);
int tty_write(char* buf, size_t len);
int tty_putc(char c);
int tty_puts(char* s);

#endif

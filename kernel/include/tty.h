#ifndef _TTY_H
#define _TTY_H

int tty_init(void);
int tty_write(char* buf, size_t len);
int tty_putc(char c);
int tty_puts(char* s);

#endif

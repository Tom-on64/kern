#ifndef _SERIAL_H
#define _SERIAL_H

#include <kernel.h>

#define COM1	0x3f8
#define COM2	0x2f8
#define COM3	0x3e8
#define COM4	0x2e8

// Buffer for (v)debugf()
#define DF_BUF_LEN	2048

int  serial_init(int dev);
int  serial_txem(int dev);
void serial_send(int dev, char data);
int  serial_rcvd(int dev);
char serial_recv(int dev);
void serial_write(int dev, const char* buf, size_t len);
void serial_puts(int dev, const char* s);
void serial_cls(int dev);

int debugf(const char* fmt, ...);
int vdebugf(const char* fmt, va_list args);

#endif

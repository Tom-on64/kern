#ifndef _SERIAL_H
#define _SERIAL_H

#include <kernel.h>

#define COM1	0x3f8
#define COM2	0x2f8
#define COM3	0x3e8
#define COM4	0x2e8

int  serial_init(int dev);
int  serial_txem(int dev);
void serial_send(int dev, char data);
int  serial_rcvd(int dev);
char serial_recv(int dev);
void serial_write(int dev, char* buf, size_t len);
void serial_print(int dev, char* s);

#endif

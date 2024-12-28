#ifndef _DRIVERS_SERIAL_H
#define _DRIVERS_SERIAL_H

#include <stdarg.h>

#define PORT_COM1   0x3f8
#define PORT_COM2   0x2f8
#define PORT_COM3   0x3e8
#define PORT_COM4   0x2e8

// IRQ4 -> COM1, IRQ3 -> COM2
#define SERIAL_IRQ  4

int serialEnable(int port);
int setupSerial();
void serialWrite(int port, char c);
char serialRead(int port);
char serialReadAsync(int port);

int debugf(const char* fmt, ...);
int vdebugf(const char* fmt, va_list va);

#endif

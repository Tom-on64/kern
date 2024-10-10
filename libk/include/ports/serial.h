#ifndef PORTS_SERIAL_H
#define PORTS_SERIAL_H

#include <stdint.h>

#define PORT_COM1   0x3f8
#define PORT_COM2   0x2f8
#define PORT_COM3   0x3e8
#define PORT_COM4   0x2e8
#define PORT_COM5   0x5f8
#define PORT_COM6   0x4f8
#define PORT_COM7   0x5e8
#define PORT_COM8   0x4e8

int setupSerial(uint32_t port);

int serialTransmitEmpty(uint32_t port);
int serialRecieved(uint32_t port);

void serialWrite(uint32_t port, char c);
void serialPrint(uint32_t port, char* s);
char serialRead(uint32_t port);

#endif

#ifndef PORTS_IO_H
#define PORTS_IO_H

#include <stdint.h>

// Port functions
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t data);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t data);
void ioWait();

#endif

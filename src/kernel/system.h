#ifndef SYSTEM_H
#define SYSTEM_H

#include "stdint.h"

// Port functions
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t data);

uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t data);

// Memory functions
char* memcopy(char* src, char* dst, uint16_t length);
char* memset(char* dst, char data, uint16_t length);

// Assembly stuff
void cli();
void sti();
void hlt();

#endif

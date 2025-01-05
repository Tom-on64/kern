#ifndef _KBD_H
#define _KBD_H

#include "stdint.h"

#define PS2_DATA_PORT	0x60
#define PS2_STAT_PORT	0x64

// Scancodes
#define SC_ENTER	0x1C
#define SC_EXTENDED	0xE0
// Extended scancodes
#define SCE_ARROW_UP	0x48
#define SCE_ARROW_DOWN	0x50

int keyboardInit();
uint8_t getScancode();

#endif

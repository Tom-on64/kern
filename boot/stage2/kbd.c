#include "stdint.h"
#include "port.h"
#include "kbd.h"

// TODO: Maybe do some setup?
int keyboardInit() { return 0; }

uint8_t getScancode() {
	while (inb(PS2_STAT_PORT) & 0x01 == 0);
	return inb(PS2_DATA_PORT);
}


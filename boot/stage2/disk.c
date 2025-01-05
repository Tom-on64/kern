#include "stdint.h"
#include "port.h"
#include "disk.h"

void readSectors(uint32_t lba, size_t count, void* dst) {
	outb(PIO_PORT + 6, ((lba >> 24) & 0x0f) | 0xe0);
	outb(PIO_PORT + 2, count);
	outb(PIO_PORT + 3, lba & 0xff);
	outb(PIO_PORT + 4, (lba >> 8) & 0xff);
	outb(PIO_PORT + 5, (lba >> 16) & 0xff);
	outb(PIO_PORT + 7, PIO_CMD_READ);

	uint16_t* buf = (uint16_t*)dst;

	for (size_t i = 0; i < count; i++) {
		while (inb(PIO_PORT + 7) & (1 << 7));	// Wait until BSY is cleared
		for (size_t j = 0; j < SECTOR_SIZE / 2; j++) *buf++ = inw(PIO_PORT);
		for (uint8_t wait = 0; wait < 4; wait++) inb(PIO_ALT_STATUS);
	}
}

/* TODO */
void writeSectors(uint32_t lba, size_t count, void* src) {}


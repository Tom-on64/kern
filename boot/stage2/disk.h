#ifndef DISK_H
#define DISK_H

#include "stdint.h"
#include "defs.h"

#define SECTOR_SIZE	512
#define PIO_PORT	0x1f0
#define PIO_ALT_STATUS	0x3f6
#define PIO_MASTER	0xa0
#define PIO_CMD_READ	0x20
#define PIO_CMD_WRITE	0x30
#define PIO_CMD_FLUSH	0	// TODO

#define BOOTABLE	0x80

#define LBA_TO_OFFSET(_a)	((_a) * SECTOR_SIZE)

struct partition {
	uint8_t bootable;
	uint8_t startCHS[3];
	uint8_t type;
	uint8_t endCHS[3];
	uint32_t startLBA;
	uint32_t size;
} __packed;

struct MBR {
	uint8_t bootstrap[446];
	struct partition partitions[4];
	uint16_t signature;
} __packed;

void readSectors(uint32_t lba, size_t count, void* buffer);
void writeSectors(uint32_t lba, size_t count, void* buffer);

#endif

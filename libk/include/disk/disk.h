#ifndef DISK_DISK_H
#define DISK_DISK_H

#include <stdint.h>

#define PIO_PORT 0x1f0
#define READ_COMMAND 0x20
#define WRITE_COMMAND 0x30
#define CACHE_FLUSH 0xE7

void diskRead(uint32_t lbaAddress, uint8_t sectorCount, char* dst);
void diskWrite(uint32_t lbaAddress, uint8_t sectorCount, char* src);

#endif

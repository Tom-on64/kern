#include "disk.h"
#include "stdint.h"
#include "system.h"

#define PIO_PORT 0x1f0

void diskRead(uint8_t sector, uint8_t sectorCount, uint8_t drive, char* dst) {
    outb(PIO_PORT + 6, (drive & 0x0f) | 0xa0); // Head and drive number port
    outb(PIO_PORT + 2, sectorCount); // Sector count
    outb(PIO_PORT + 3, sector); // Sector to start reading at
    outb(PIO_PORT + 4, 0); // Cylinder number low
    outb(PIO_PORT + 5, 0); // Cylinder number high
    outb(PIO_PORT + 7, 0x20); // Command register, 0x20 - Read with retry
    
    while (inb(PIO_PORT + 7) == 8); // Status register, wait until ready

    uint32_t i = 512 * sectorCount;
    while (i > 0) {
        uint16_t data = inw(PIO_PORT); // Data port
        *dst++ = (uint8_t)(data);
        *dst++ = (uint8_t)(data >> 8);
        i--;
    }
}

void diskWrite(uint8_t sector, uint8_t sectorCount, uint8_t drive, const char* src);


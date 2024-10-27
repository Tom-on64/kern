#include <drivers/disk.h>
#include <cpu/system.h>
#include <stdint.h>

void diskRead(uint32_t lbaAddress, uint8_t sectorCount, char* dst) {
    outb(PIO_PORT + 6, ((lbaAddress >> 24) & 0x0f) | 0xe0); // 6 - Use LBA; 4 - Drive number; 3-0 - Highest lba address nibble
    outb(PIO_PORT + 2, sectorCount); // Sector count register
    outb(PIO_PORT + 3, (uint8_t)(lbaAddress)); // Lowest lba address byte
    outb(PIO_PORT + 4, (uint8_t)(lbaAddress >> 8)); // Middle lba address byte
    outb(PIO_PORT + 5, (uint8_t)(lbaAddress >> 16)); // High lba address byte
    outb(PIO_PORT + 7, READ_COMMAND); // Command register
    
    uint16_t* address = (uint16_t*)dst;

    for (uint8_t i = sectorCount; i > 0; i--) {
        while (inb(PIO_PORT + 7) & (1 << 7)); // Poll status register untill BSY bit is clear

        for (size_t j = 0; j < 256; j++) {
            *address++ = inw(PIO_PORT); // Read 256 words (512 Bytes = 1 Sector)
        }

        for (uint8_t delay = 0; delay < 4; delay++) { // 400ns delay
            inb(0x3f6); // Alternate status register
        }
    }
}

void diskWrite(uint32_t lbaAddress, uint8_t sectorCount, char* src) {
    outb(PIO_PORT + 6, ((lbaAddress >> 24) & 0x0f) | 0xe0); // 6 - Use LBA; 4 - Drive number; 3-0 - Highest lba address nibble
    outb(PIO_PORT + 2, sectorCount); // Sector count register
    outb(PIO_PORT + 3, (uint8_t)(lbaAddress)); // Lowest lba address byte
    outb(PIO_PORT + 4, (uint8_t)(lbaAddress >> 8)); // Middle lba address byte
    outb(PIO_PORT + 5, (uint8_t)(lbaAddress >> 16)); // High lba address byte
    outb(PIO_PORT + 7, WRITE_COMMAND); // Command register
    
    uint16_t* address = (uint16_t*)src;

    for (uint8_t i = sectorCount; i > 0; i--) {
        while (inb(PIO_PORT + 7) & (1 << 7)); // Poll status register untill BSY bit is clear

        for (size_t j = 0; j < 256; j++) {
            outw(PIO_PORT, *address++); // Write 256 words (512 Bytes = 1 Sector)
        }

        for (uint8_t delay = 0; delay < 4; delay++) { // 400ns delay
            inb(0x3f6); // Alternate status register
        }
    }
    
    outb(PIO_PORT + 7, CACHE_FLUSH); // Cache flush
    while (inb(PIO_PORT + 7) & (1 << 7)); // Poll status register untill BSY bit is clear
}


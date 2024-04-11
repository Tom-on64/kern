#ifndef DRIVE_H
#define DRIVE_H

#include "stdint.h"

extern uint8_t driveNum;

void diskRead(uint32_t lbaAddress, uint8_t sectorCount, char* dst);
// TODO: void diskWrite(uint8_t sector, uint8_t sectorCount, uint8_t drive, const char* src);

#endif

#ifndef DRIVE_H
#define DRIVE_H

#include "stdint.h"

extern uint8_t driveNum;

void diskRead(uint32_t lbaAddress, uint8_t sectorCount, char* dst);
void diskWrite(uint32_t lbaAddress, uint8_t sectorCount, char* src);

#endif

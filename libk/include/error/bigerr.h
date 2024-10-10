#ifndef ERROR_BIGERR_H
#define ERROR_BIGERR_H

/*
 * Something went VERY wrong. 
 * Eg. kernel.bin is missing, no filesystem, Virtual Memory failed, etc.
 */

#include <stdint.h>

void rect(uint32_t c, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void bigError(uint32_t err);

#endif

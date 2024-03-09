#ifndef IDT_H
#define IDT_H

#include "stdint.h"

void setupIdt();
void idtSetGate(uint8_t i, uint32_t offset, uint16_t selector, uint8_t typeAttr);
extern void idtLoad();

#endif


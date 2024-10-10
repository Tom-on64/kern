#ifndef INTERRUPT_PIC_H
#define INTERRUPT_PIC_H

#include <stdint.h>

// PIC I/O Ports
#define PIC_1_CMD  0x20
#define PIC_1_DATA 0x21
#define PIC_2_CMD  0xA0
#define PIC_2_DATA 0xA1

// IRQs
#define NEW_IRQ_0 0x20 // IRQ 0 : int 32
#define NEW_IRQ_8 0x28 // IRQ 8 : int 40

// Commands
#define PIC_EOI 0x20 // End Of Interrupt
#define PIC_DISABLE 0xff

void sendPicEOI(uint8_t irq);
void disableIrqs();
void setIrqMask(uint8_t irq);
void unsetIrqMask(uint8_t irq);
void remapPic();

#endif

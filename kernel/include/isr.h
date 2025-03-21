#ifndef _ISR_H
#define _ISR_H

#include <kernel.h>

// Interrupt frame (ie. what's on the stack on interrupt)
struct isr_intFrame {
	// Pushed by OS
	uint32_t esp, ebx, ecx, edx, esi, edi, ebp;
	uint32_t ds, es, fs, gs, eax;
	uint32_t interrupt, error;

	// Pushed by CPU
	uint32_t eip, cs, eflags, user_esp, user_ss;
};
typedef void (*isr_handlerPtr)(struct isr_intFrame* iframe);

int isr_init(void);
void isr_registerIRQ(uint8_t i, isr_handlerPtr handler);
int isr_handleInterrupt(struct isr_intFrame iframe);

// Assembly definitions (isr.s)
extern void* isr_redirectTable[48]; // ISR Table
extern void isr128(); // int 0x80 - syscall

#endif

#ifndef _ISR_H
#define _ISR_H

#include <kernel.h>

// Interrupt frame (ie. what's on the stack on interrupt)
struct isr_int_frame {
	// Pushed by OS
	uint32_t esp, ebx, ecx, edx, esi, edi, ebp;
	uint32_t ds, es, fs, gs, eax;
	uint32_t interrupt, error;

	// Pushed by CPU
	uint32_t eip, cs, eflags, user_esp, user_ss;
};
typedef void (*isr_handler_ptr)(struct isr_int_frame* iframe);

int isr_init(void);
void isr_register(uint8_t i, isr_handler_ptr handler);
int isr_handle_interrupt(struct isr_int_frame iframe);

// Assembly definitions (isr.s)
extern void* isr_redirect_table[48]; // ISR Table
extern void isr128(); // int 0x80 - syscall

#endif

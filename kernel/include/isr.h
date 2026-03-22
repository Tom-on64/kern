#ifndef _ISR_H
#define _ISR_H

#include <kernel.h>

#define ISR_COUNT	16

// Interrupt frame (ie. what's on the stack on interrupt)
struct isr_int_frame {
	// Pushed by OS
	uint64_t ds; // + es
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
	uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
	uint64_t interrupt, error;

	// Pushed by CPU
	uint64_t rip, cs, rflags, user_rsp, user_ss;
} __packed;
typedef void (*isr_handler_ptr)(struct isr_int_frame* iframe);

int  isr_init(void);
void isr_register(uint8_t i, isr_handler_ptr handler);
void isr_handle_interrupt(size_t rsp);

// Assembly definitions (isr.s)
extern void  isr_return(void);
extern void* isr_redirect_table[48]; // ISR Table
extern void  isr128(void); // int 0x80 - syscall
extern void  isr255(void); // int 0xFF - 

#endif

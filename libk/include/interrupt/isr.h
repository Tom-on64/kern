#ifndef INTERRUPT_ISR_H
#define INTERRUPT_ISR_H

#include <stdint.h>

typedef struct {
    // Pushed by OS
    uint32_t esp, ebx, ecx, edx, esi, edi, ebp;
    uint32_t ds, es, fs, gs, eax;
    uint32_t interrupt, error;

    // Pushed by CPU
    uint32_t eip, cs, eflags, user_esp, user_ss;
} intFrame_t;
typedef void (*HandlerPtr)(intFrame_t* iframe);

void setupIsrs();
int handleInterrupt(intFrame_t iframe);
void registerIrq(uint8_t id, HandlerPtr handler);

// ISR Table
extern void* asm_isrRedirectTable[48];

// Int 0x80 - Syscall
extern void isr128();

#endif

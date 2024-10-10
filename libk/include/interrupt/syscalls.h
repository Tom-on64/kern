#ifndef INTERRUPT_SYSCALLS_H
#define INTERRUPT_SYSCALLS_H

#include <interrupt/idt.h>
#include <stdint.h>

typedef struct {
    uint32_t esp;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t ds;
    uint32_t es;
    uint32_t fs;
    uint32_t gs;
    uint32_t eax;
} __attribute__ ((packed)) sys_regs_t;

__attribute__ ((naked)) void syscallHandler(intFrame_t* iframe);

#endif

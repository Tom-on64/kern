#include <syscalls/syscalls.h>
#include <memory/addresses.h>
#include <cpu/isr.h>
#include <stdint.h>

int sys_sleep(intFrame_t* iframe) {
    uint32_t* sleepTimerTicks = (uint32_t*)SLEEP_TIMER;
    *sleepTimerTicks = iframe->ebx;

    while (*sleepTimerTicks > 0) { __asm__ volatile ("sti; hlt"); }

    return 0;
}


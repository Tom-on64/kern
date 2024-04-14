#ifndef TIMER_H
#define TIMER_H

#include "stdint.h"
#include "idt.h"
#include "irq.h"
#include "system.h"

#define INPUT_CLK 1193180

uint32_t timerTicks = 0;

void timerHandler() {
    timerTicks++;
}

void setupTimer() {
    installIrqHandler(0, timerHandler);
}

// Source: https://en.wikibooks.org/wiki/X86_Assembly/Programmable_Interval_Timer
void timerPhase(uint32_t hz) {
    uint16_t div = INPUT_CLK / hz;

    outb(0x43, 0x36); // Tell PIT that we're setting channel 0
    outb(0x40, div & 0xff); // Low byte
    outb(0x40, div >> 8); // High byte
}

void sleep(uint32_t ticks) {
    uint32_t eticks = ticks + timerTicks;
    
    while (timerTicks != eticks); // Should avoid overflows unlike '<'
}


#endif

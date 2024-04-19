#ifndef TIMER_H
#define TIMER_H

#include "stdint.h"
#include "idt.h"
#include "irq.h"
#include "system.h"

#include "screen.h"

#define INPUT_CLK 1193180

uint32_t timerTicks = 0;

void timerHandler() {
    timerTicks++;

    // TODO: Improve the cursor rendering
    if (timerTicks % 500 == 0) putcAt(127, cursor.x, cursor.y);
    else if (timerTicks % 250 == 0) putcAt(' ', cursor.x, cursor.y);
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
    
    while (timerTicks != eticks); // Should ignore overflows unlike '<'
}


#endif

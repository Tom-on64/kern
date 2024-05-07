#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <idt.h>
#include <pic.h>
#include <screen.h>

#define CHANNEL_0 0x40 // Offset it by 1 or 2 for ch1 and ch2
#define COMMAND   0x43
#define INPUT_CLK 1193182

static uint32_t* timerTicks = (uint32_t*)0x1000;

__attribute__ ((interrupt))
void timerHandler(intFrame_t* iframe) {
    *timerTicks += 1;

    // TODO: Improve the cursor rendering
    if (*timerTicks % 500 == 0) putcAt(' ', cursor.x, cursor.y);
    else if (*timerTicks % 250 == 0) putcAt(127, cursor.x, cursor.y);

    sendPicEOI(0);
}

void setPitPhase(uint8_t channel, uint8_t opMode, uint16_t hz) {
    if (channel > 2 || opMode > 7) { return; }
    __asm__ volatile ("cli");

    // Send command - bits 7-6 channel, 5-4 Access mode, 3-1 operating mode, 0 - Should be zero in x86
    outb(COMMAND, ((channel << 6) | (3 << 4) | (opMode << 1)));

    // Send frequency
    uint16_t freq = INPUT_CLK / hz; // I solved an equation to figure this out
    outb(CHANNEL_0 + channel, (uint8_t)freq);
    outb(CHANNEL_0 + channel, (uint8_t)(freq >> 8));
    
    __asm__ volatile ("sti");
}

void setupTimer() { // Sets up IRQ 0 - PIT
    idtSetGate(0x20, timerHandler, INT_GATE_FLAGS);
    unsetIrqMask(0);
    setPitPhase(0, 2, 1000); // Default setup: 1000Hz - 1 tick per 1ms
}

#endif

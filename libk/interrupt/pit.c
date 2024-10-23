#include <interrupt/pit.h>
#include <interrupt/isr.h>
#include <tty/tty.h>
#include <memory/addresses.h>
#include <ports/io.h>
#include <stdint.h>

void timerHandler(intFrame_t*) {
    static uint32_t cursorTicks = 0;
    cursorTicks++;
    
    if (!console->hideCursor) { // TODO: Make a better cursor drawing thing
        if (cursorTicks % 500 == 0) toggleCursor();
    }

    if (*(uint32_t*)SLEEP_TIMER > 0) { --(*(uint32_t*)SLEEP_TIMER); }
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


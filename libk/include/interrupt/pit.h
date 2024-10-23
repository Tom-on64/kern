#ifndef INTERRUPT_PIT_H
#define INTERRUPT_PIT_H

#include <interrupt/isr.h>
#include <stdint.h>

#define CHANNEL_0 0x40 // Offset it by 1 or 2 for ch1 and ch2
#define COMMAND   0x43
#define INPUT_CLK 1193182

void timerHandler(intFrame_t*);
void setPitPhase(uint8_t channel, uint8_t opMode, uint16_t hz);

#endif

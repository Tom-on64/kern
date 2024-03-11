#ifndef TIMER_H
#define TIMER_H

#include "stdint.h"

#define INPUT_CLK 1193180

void setupTimer();
void sleep(uint32_t ticks);
void timerPhase(uint32_t hz);

#endif

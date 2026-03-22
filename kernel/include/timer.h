#ifndef _TIMER_H
#define _TIMER_H

#include <isr.h>

#define PIT_DIV 1193180
#define PIT_CH0	0x40
#define PIT_CH1	0x41
#define PIT_CH2	0x42
#define PIT_CMD	0x43

#define TIMER_FREQ	1000

int  timer_init(void);
void timer_wait(size_t ticks);

int hpet_init(void);

// TODO: rtc_init(void);

#endif

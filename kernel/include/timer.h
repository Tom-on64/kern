#ifndef _TIMER_H
#define _TIMER_H

#include <isr.h>

#define PIT_DIV 1193180
#define PIT_CH0	0x40
#define PIT_CH1	0x41
#define PIT_CH2	0x42
#define PIT_CMD	0x43

int  timer_init(void);
void timer_wait(size_t ticks);

int  pit_init(void);
void pit_phase(uint8_t ch, uint8_t mode, uint32_t hz);
void pit_handler(struct isr_int_frame* iframe);

// TODO: rtc_init(void);
// TODO: hpet_init(void);

#endif

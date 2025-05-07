#include <kernel.h>
#include <system.h>
#include <sched.h>
#include <isr.h>
#include <tty.h>

#include <timer.h>

size_t timer_ticks = 0;

int timer_init(void) {
	// TODO: Check what timer we can use and select the best one
	return pit_init();
}

void timer_wait(size_t t) {
	size_t req = timer_ticks + t;
	while (req != timer_ticks);
}

int pit_init(void) {
	isr_register(0, pit_handler);
	pit_phase(0, 2, TIMER_FREQ);
	return 0;
}

void pit_phase(uint8_t ch, uint8_t mode, uint32_t hz) {
	if (ch > 2 || mode > 7) return;
	
	cli();

	outb(PIT_CMD, ((ch << 6) | (3 << 4) | (mode << 1)));

	uint32_t div = PIT_DIV / hz;
	outb(PIT_CH0 + ch, div & 0xFF);
	outb(PIT_CH0 + ch, div >> 8);
	debugf("[pit] Set PIT divisor to %d (%d Hz).\n", div, hz);

	sti();
}

void pit_handler(struct isr_int_frame* iframe) {
	(void)iframe; // We must take it as an argument even if we don't use it
	timer_ticks++;

	// TODO: This assumes the timer is at 1000 Hz
	if (timer_ticks % SCHED_QUANTUM == 0) schedule();
}


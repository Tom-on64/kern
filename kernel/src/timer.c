#include <kernel.h>
#include <system.h>
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
	pit_phase(0, 2, 1000);
	return 0;
}

void pit_phase(uint8_t ch, uint8_t mode, uint32_t hz) {
	if (ch > 2 || mode > 7) return;
	
	__asm__ volatile ("cli");

	outb(PIT_CMD, ((ch << 6) | (3 << 4) | (mode << 1)));

	uint32_t div = PIT_DIV / hz;
	debugf("[PIT] Setting PIT divisor to %d...\n", div);
	outb(PIT_CH0 + ch, div & 0xFF);
	outb(PIT_CH0 + ch, div >> 8);

	__asm__ volatile ("sti");
}

void pit_handler(struct isr_int_frame* iframe) {
	timer_ticks++;

	if (timer_ticks % 1000 == 0) tty_puts(".");
}


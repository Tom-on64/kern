#include <kernel.h>

__noreturn
void kmain(void) {
	*(char*)0xb8000 = 'a';
	while (1) __asm__ volatile ("movl $69, %eax");
}


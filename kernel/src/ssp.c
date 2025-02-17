/*
 * Stack Smash Protector
 * Source: https://wiki.osdev.org/Stack_Smashing_Protector
*/
#include <kernel.h>

// TODO: Make this randomized
#define STACK_CHK_GUARD	0x27a1abd8

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;
__noreturn void __stack_chk_fail(void) {
	panic("Stack smashing detected!");
}


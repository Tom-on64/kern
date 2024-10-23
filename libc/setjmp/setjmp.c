#include <setjmp.h>

int setjmp(jmp_buf env) {
    __asm__ volatile (
        "mov %%ebp, %0\n"   // Save BP
        "mov %%esp, %1\n"   // Save SP
        "call 1f\n"         // Call label to push PC to the stack
        "1:\n"              // Label
        "pop %2\n"          // Save PC
        : "=m"(env._bp), "=m"(env._sp), "=m"(env._pc)  // Locations
    );
    return 0;
}

void longjmp(jmp_buf env, int value) {
    if (value == 0) return;

    __asm__ volatile (
        "mov %0, %%ebp\n"   // Restore BP
        "mov %1, %%esp\n"   // Restore SP
        "mov %2, %%eax\n"   // Put return address into A
        "jmp *%%eax\n"      // Jump to A
        :: "m"(env._bp), "m"(env._sp), "m"(env._pc)
    );
    // This point is unreachable :)
}


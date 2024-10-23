#include <error/panic.h>

void panic() {
    // TODO: print a message
    while (1) { __asm__ volatile ("cli; hlt"); }
}


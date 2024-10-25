#include <time.h>
#include <syscall.h>
#include <stdint.h>

// Seconds sleep
void sleep(size_t s) {
    syscall(SYS_SLEEP, s*1000, 0, 0);
}

// Millisecond sleep
void msleep(size_t ms) {
    syscall(SYS_SLEEP, ms, 0, 0);
}

// Microsecond sleep
void usleep(size_t us) { /* TODO */ }


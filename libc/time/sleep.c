#include <time.h>
#include <syscall.h>
#include <stdint.h>

// Seconds sleep
void sleep(size_t s) { _sleep(s*1000); }

// Millisecond sleep
void msleep(size_t ms) { _sleep(ms); }

// Microsecond sleep
void usleep(size_t us) { /* TODO */ }


#ifndef TIME_H
#define TIME_H

#include <syscall.h>
#include <stdint.h>

void sleep(uint32_t ms) {
    syscall(SYS_SLEEP, ms, 0, 0);
}

#endif

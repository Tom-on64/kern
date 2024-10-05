#include <time.h>
#include <syscall.h>
#include <stdint.h>

void sleep(uint32_t ms) {
    syscall(SYS_SLEEP, ms, 0, 0);
}

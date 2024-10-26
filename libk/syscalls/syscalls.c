#include <syscalls/syscalls.h>
#include <interrupt/isr.h>
#include <error/debug.h>
#include <syscall.h>

// System call table
int (*syscalls[SYSCALL_COUNT])(intFrame_t*) = {
    [SYS_EXIT]      = sys_exit,
    [SYS_READ]      = sys_read, 
    [SYS_WRITE]     = sys_write,
    [SYS_OPEN]      = sys_open,
    [SYS_CLOSE]     = sys_close,
    [SYS_SEEK]      = sys_seek,
    [SYS_MALLOC]    = sys_malloc,
    [SYS_FREE]      = sys_free,
    [SYS_SLEEP]     = sys_sleep,
};
    
// Syscall handler
int syscallHandler(intFrame_t* iframe) {
    if (iframe->eax > SYSCALL_COUNT-1) { // Syscall doesn't exist
        debugf("Error: Syscall(%d) does not exist!\n", iframe->eax);
        return -1;
    }

    return syscalls[iframe->eax](iframe);
}


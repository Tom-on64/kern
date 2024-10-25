#include <stdlib.h>
#include <syscall.h>

// Exits without cleaning up
void _Exit(int status) { _exit(status); }

void exit(int status) {
    // TODO: Close open files
    // TODO: Free malloced buffers

    _Exit(status);
}


#ifndef SYSCALLNUMS_H
#define SYSCALLNUMS_H

#define MAX_SYSCALLS 9

typedef enum {
    SYS_RESERVED,
    SYS_SLEEP, 
    SYS_GETS,
    SYS_WRITE,
    SYS_OPEN,
    SYS_CLOSE,
    SYS_MALLOC,
    SYS_FREE,
    SYS_SEEK,
} SYSCALL_NUMBERS;

typedef enum {
    O_CREAT     = 0x01,  // Create if does not exist
    O_RDONLY    = 0x02,  // Read only
    O_WRONLY    = 0x04,  // Write only
    O_RDWR      = 0x08,  // Read and write
    O_APPEND    = 0x10, // Always writes at the end (appends)
    O_TRUNC     = 0x20, // Truncate file size and position to 0 on Open
    O_BIN       = 0x40, // Read bytes, not text
} OFLAGS;

#endif

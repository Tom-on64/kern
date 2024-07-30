#ifndef SYSCALLNUMS_H
#define SYSCALLNUMS_H

#define MAX_SYSCALLS 9

// TODO: Add malloc() and free() syscall wrappers??

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
    O_CREAT     = 0x1,  // Create if does not exist
    O_RDONLY    = 0x2,  // Read only
    O_WRONLY    = 0x4,  // Write only
    O_RDWR      = 0x8,  // Read and write
    O_APPEND    = 0x10, // Always writes at the end (appends)
    O_TRUNC     = 0x20, // Truncate file size and position to 0 on Open
} OFLAGS;

int write(int fd, const void* buf, unsigned int len) {
    int result = -1;
    __asm__ volatile ("int $0x80" : "=a"(result) : "a"(SYS_WRITE), "b"(fd), "c"(buf), "d"(len));
    return result;
}

int open(const char* path, int oflag) {
    int result = -1;
    __asm__ volatile ("int $0x80" : "=a"(result) : "a"(SYS_OPEN), "b"(path), "c"(oflag));
    return result;
}

#endif

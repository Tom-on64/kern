#ifndef STDIO_H
#define STDIO_H

void print(const char* s) { // Use Syscall(1) - Puts()
    __asm__ volatile ("int $0x80" : : "a"(1), "b"(s));
}

void read(char* s) { // Use Syscall(2) - Gets()
    __asm__ volatile ("int $0x80" : : "a"(2), "b"(s));
}

#endif

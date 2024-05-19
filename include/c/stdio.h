#ifndef STDIO_H
#define STDIO_H

void print(const char* s) { // Use Syscall(1) - Puts()
    __asm__ volatile ("int $0x80" : : "a"(1), "b"(s));
}

char* read() { // Use Syscall(2) - Gets()
    char* s;
    __asm__ volatile ("int $0x80" : "=b"(s) : "a"(2));
    return s;
}

#endif

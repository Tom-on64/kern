#ifndef STDIO_H
#define STDIO_H

#include <syscall.h>
#include <string.h>

// TODO: Make a proper file descriptor table
#define STDOUT 1

void print(const char* s) {
    write(STDOUT, s, strlen(s));
}

void read(char* s) { 
    print("\x1b[1M[ ERROR: Read() Syscall not found! ]\n");
}

#endif

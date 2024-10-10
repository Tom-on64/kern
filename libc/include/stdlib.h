#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>
#include <syscall.h>

void* malloc(const uint32_t size);
void free(const void* ptr);

#endif

#include <stdlib.h>

// taken from glibc
#define RAND_A  1103515245
#define RAND_C  12345

static size_t _next = 1;

int rand(void) {
    _next = (RAND_A * _next + RAND_C) & RAND_MAX;
    return (int)(_next % (size_t)(RAND_MAX + 1));
}

void srand(size_t seed) { _next = seed; }


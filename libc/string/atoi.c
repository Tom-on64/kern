#include <string.h>

unsigned int atoi(const char* str) {
    unsigned int res = 0;

    // TODO: Add error checking

    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str++ - '0');
    }

    return res;
}

#include <time.h>

__attribute__ ((section ("entry")))
void main() {
    *(char*)(0x2000) = 'H';
    *(char*)(0x2001) = 'e';
    *(char*)(0x2002) = 'l';
    *(char*)(0x2003) = 'l';
    *(char*)(0x2004) = 'o';
    *(char*)(0x2005) = '!';
    *(char*)(0x2006) = '\0';
    __asm__ volatile ("int $0x80" : : "a"(1), "b"(0x2000));
    sleep(4500); // Wait 4.5 seconds

    return;
}

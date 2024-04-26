#include "stdint.h"
#include "screen.h"

__attribute__ ((section ("entry")))
void main() {
    uint32_t wait = 0xffffffff;

    putc('a');
    
    while (wait--);

    return;
}

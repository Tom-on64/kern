#include "stdint.h"
#include "screen.h"

__attribute__ ((section ("entry")))
void main() {
    uint32_t wait = 0x1fffffff;

    print("kern.\n\n");
    
    while (wait--);

    return;
}

#include "screen.h"
#include "idt.h"
#include "isrs.h"

void main() {
    setupIdt();
    setupIsrs();
    clear(0x0f);

    print("kern.\n\n", 0x0f);
}


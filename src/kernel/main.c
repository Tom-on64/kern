#include "screen.h"
#include "idt.h"
#include "isrs.h"
#include "irq.h"

void main() {
    setupIdt();
    setupIsrs();
    setupIrqs();

    clear(0x0f);
    print("kern.\n\n", 0x0f);
}


#include "screen.h"
#include "idt.h"
#include "isrs.h"
#include "irq.h"

void main() {
    // Interrupts
    setupIdt();
    setupIsrs();
    setupIrqs();

    clear(0x0f);
    print("kern.\n", 0x0f);
}


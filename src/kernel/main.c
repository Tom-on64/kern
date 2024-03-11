#include "screen.h"
#include "idt.h"
#include "isrs.h"
#include "irq.h"
#include "keyboard.h"
#include "system.h"

void main() {
    // Interrupts
    setupIdt();
    setupIsrs();
    setupIrqs();
    sti();

    setupKeyboard();

    clear(0x0f);
    print("kern.\n\n", 0x0f);
}


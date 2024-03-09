#include "screen.h"
#include "idt.h"

void main() {
    setupIdt();
    clear(0x0f);

    print("kern.\n\n", 0x0f);
    print("> ", 0x0e);
}


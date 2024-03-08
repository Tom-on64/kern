#include "screen.h"
#include "idt.h"

void main() {
    setupIdt();
    print("kern.\n", 0x0f);
}


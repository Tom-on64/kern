#include "screen.h"
#include "idt.h"
#include "isrs.h"
#include "irq.h"
#include "keyboard.h"
#include "timer.h"
#include "system.h"
#include "disk.h"
#include "string.h"

void main() {
    // Interrupts
    setupIdt();
    setupIsrs();
    setupIrqs();
    sti();

    setupKeyboard();
    setupTimer();
    timerPhase(100); // 1 tick per 10ms
    enableCursor(14, 15);

    clear(0x0f);
    print("kern.\n\n", 0x0f);

    while (1) {
        print("> ", 0x0e);
        char* input = read('\n', 0x0f);
        if (*input == '\n') continue;
        print(input, 0x0b);
    }
}


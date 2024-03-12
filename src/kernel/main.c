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
    print("Drive: 0x", 0x0f);
    print(itoa(driveNum, 16), 0x0f);
    putc('\n', 0x00);
}


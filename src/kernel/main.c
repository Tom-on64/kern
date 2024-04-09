#include "screen.h"
#include "idt.h"
#include "isrs.h"
#include "irq.h"
#include "keyboard.h"
#include "timer.h"
#include "system.h"
#include "disk.h"
#include "string.h"

#define PROMPT "#> "

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
    
    // Run Interactive Shell Program
    // TODO: Make it in another file

    while (1) {
        print(PROMPT, 0x0a);
        char* input = read('\n', 0x0f);
        
        if (input[0] == '\n') continue;
        
        char* args;
        if ((args = strchr(input, ' ')) == NULL) {
            args = strchr(input, '\n');
        }

        *args = '\0';
        args++;

        if (strcmp(input, "clear") == 0) {
            clear(0x0f);
        } else if (strcmp(input, "echo") == 0) {
            print(args, 0x0f);
        } else if (strcmp(input, "exit") == 0) {
            break;
        } else if (strcmp(input, "help") == 0) {
            print("Available Commands:\n", 0x0f);
            print(" clear      | Clears the screen\n", 0x0f);
            print(" echo       | Prints a message to stdout\n", 0x0f);
            print(" exit       | Exits shell\n", 0x0f);
            print(" help       | Prints this message\n", 0x0f);
            // TODO: print(" ls         | Lists all available files\n", 0x0f);
            // TODO: print(" reboot     | Reboots the system\n", 0x0f);
            print(" test       | Tests the latest feature [DEBUG]\n", 0x0f);
        } else if (strcmp(input, "test") == 0) {
            char buf[512];
            print("Reading from drive 0x", 0x0f);
            print(itoa(driveNum, 16), 0x0f);
            print("...\n", 0x0f);
            diskRead(22, 1, driveNum, buf);

            print(buf, 0x0f);
            putc('\n', 0x0f);
        } else {
            print("Command not found: ", 0x0c);
            print(input, 0x0c);
            putc('\n', 0x0f);
        }
    }
}

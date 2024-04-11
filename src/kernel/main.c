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

        *args = '\0'; // Separate command from args
        args++;

        if (strcmp(input, "clear") == 0) {
            clear(0x0f);
        } else if (strcmp(input, "disk") == 0) {
            static char buf[512];
            
            if (*args == '\0') {
                print("test: ", 0x0f);
                print("No arguments given\n", 0x0c);
                print("Usage: test <sector>\n", 0x0f);
                continue;
            }

            // TODO: We only support reading right now
            //char mode = *args++;
            //if (mode != 'r' || mode != 'w') {
            //    print("test: ", 0x0f);
            //    print("Invalid access mode. Use r(ead) or w(rite)\n", 0x0c);
            //    print("Usage: test <r/w> <sector> [data]\n", 0x0f);
            //    continue;
            //}

            uint32_t sector = atoi(args);
            print("Reading...\n", 0x0f);
            diskRead(sector, 1, buf);
            
            print("Contents:\n", 0x0f);
            for (size_t i = 0; i < 512; i++) {
                if (buf[i] != '\0') {
                    putc(buf[i], 0x0f);
                }
            }

            putc('\n', 0x0f);
        } else if (strcmp(input, "echo") == 0) {
            print(args, 0x0f);
        } else if (strcmp(input, "exit") == 0) {
            break;
        } else if (strcmp(input, "help") == 0) {
            print("Available Commands:\n", 0x0f);
            print(" clear      | Clears the screen\n", 0x0f);
            print(" disk       | Reads/Writes data from/to the disk\n", 0x0f);
            print(" echo       | Prints a message to stdout\n", 0x0f);
            print(" exit       | Exits shell\n", 0x0f);
            print(" help       | Prints this message\n", 0x0f);
            // TODO: print(" ls         | Lists all available files\n", 0x0f);
            // TODO: print(" reboot     | Reboots the system\n", 0x0f);
            print(" test       | Tests the latest feature [DEBUG]\n", 0x0f);
        } else if (strcmp(input, "test") == 0) {
        } else {
            print("Command not found: ", 0x0c);
            print(input, 0x0c);
            putc('\n', 0x0f);
        }
    }
}

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

char filetable[512];

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

    diskRead(1, 1, filetable); // Read the filetable and store it in memory

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

        *args++ = '\0'; // Separate command from args

        if (strcmp(input, "clear") == 0) {
            clear(0x0f);
        } else if (strcmp(input, "disk") == 0) {
            char buf[512] = { 0 };
            
            if (*args == '\0') {
                print("disk: ", 0x0f);
                print("No arguments given\n", 0x0c);
                print("Usage: disk <r/w> <sector> [data]\n", 0x0f);
                continue;
            }

            while (isSpace(*args)) { args++; } // Skip whitespace
            char mode = *args++;
            while (isSpace(*args)) { args++; }
            
            if (mode == 'r') {
                if (*args == '\0') {
                    print("disk: ", 0x0f);
                    print("Sector not specified\n", 0x0c);
                    print("Usage: disk <r/w> <sector> [data]\n", 0x0f);
                    continue;
                }

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
            } else if (mode == 'w') {
                char* data;
                if ((data = strchr(args, ' ')) == NULL) {
                    print("disk: ", 0x0f);
                    print("No data given\n", 0x0c);
                    print("Usage: disk <r/w> <sector> [data]\n", 0x0f);
                    continue;
                }

                *data++ = '\0';
                uint32_t sector = atoi(args);

                while (isSpace(*args)) { args++; } // Skip whitespace

                strcpy(buf, data);
                print("Writing...\n", 0x0f);
                diskWrite(sector, 1, buf);
            } else { 
                print("disk: ", 0x0f);
                print("Invalid access mode. Use r(ead) or w(rite)\n", 0x0c);
                print("Usage: disk <r/w> <sector> [data]\n", 0x0f);
                continue;
            }
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
            print(" ls         | Lists all available files\n", 0x0f);
            // TODO: print(" reboot     | Reboots the system\n", 0x0f);
            print(" test       | Performs tests\n", 0x0f);
        } else if (strcmp(input, "ls") == 0) {
            char* ft = filetable;
            
            while (*ft != '\0') {
                char filename[15] = { 0 };
                uint8_t offset = 0;
                for (uint8_t i = 0; i < 14; i++) {
                    if (i == 9) {
                        filename[offset++] = '.';
                        continue;
                    }
                    if (*ft != '\0') { filename[offset++] = *ft; }
                    ft++;
                }
                ft++; // RESERVED

                uint8_t sector = *ft++;
                uint8_t size = *ft++;

                if (sector < 10) putc('0', 0x0f);
                if (sector == 0) putc('0', 0x0f);
                else print(itoa(sector, 10), 0x0f);
                print(": ", 0x0f);
                print(filename, 0x0f);

                for (uint8_t i = 14 - strlen(filename); i > 0; i--) {
                    putc(' ', 0x0f);
                }

                print(" | ", 0x0f);
                if (size >> 1) {
                    print(itoa(size >> 1, 10), 0x0f);
                    if (size & 0x01) { print(".5", 0x0f); }
                    print("kB\n", 0x0f);
                } else {
                    print(itoa(size*512, 10), 0x0f);
                    print("B\n", 0x0f);
                }
            }
        } else if (strcmp(input, "test") == 0) {
            print("Running Tests...\n", 0x0f);
            uint8_t buf[512];
            
            print("Read Test .......... ", 0x0f);
            diskRead(0, 1, (char*)buf);
            if (buf[510] == 0x55 && buf[511] == 0xaa) {
                print("[ DONE ]\n", 0x0a);
            } else {
                print("[ FAIL ]\n", 0x0c);
            }
            print("Write Test ......... ", 0x0f);
            buf[510] = 0x55; buf[511] = 0xaa;
            diskWrite(0, 1, (char*)buf);
            diskRead(0, 1, (char*)buf); // Idk how else to test if the right data was written (also could break the bootloader if it fails)
            if (buf[510] == 0x55 && buf[511] == 0xaa) {
                print("[ DONE ]\n", 0x0a);
            } else {
                print("[ FAIL ]\n", 0x0c);
            }
            print("Tests Finished!\n", 0x0f);
        } else {
            char* ft = filetable;
            uint8_t found = 0;

            while (*ft != '\0') {
                char filename[15] = { 0 };
                uint8_t offset = 0;
                for (uint8_t i = 0; i < 14; i++) {
                    if (i == 9) {
                        filename[offset++] = '.';
                        continue;
                    }
                    if (*ft != '\0') { filename[offset++] = *ft; }
                    ft++;
                }

                if (strcmp(input, filename) == 0) {
                    ft++; // RESERVED
                    uint8_t sector = *ft++;
                    uint8_t size = *ft++;

                    char buf[size * 512];
                    diskRead(sector, size, buf);

                    for (size_t i = 0; i < size * 512; i++) {
                        if (buf[i] != '\0') {
                            putc(buf[i], 0x0f);
                        }
                    }
                    putc('\n', 0x0f);
                    
                    found++; // Set found to 1
                    break;
                }

                ft += 3;
            }

            if (!found) {
                print("Command not found: ", 0x0c);
                print(input, 0x0c);
                putc('\n', 0x0f);
            }
        }
    }
}


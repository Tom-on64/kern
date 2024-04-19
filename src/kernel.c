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

// TODO: Put this at a set address
char filetable[512] = { 0 };

void main() {
    // Interrupts
    setupIdt();
    setupIsrs();
    setupIrqs();
    sti();

    setupKeyboard();
    setupTimer();
    timerPhase(1000); // 1 tick per ~1ms

    // Screen setup
    diskRead(25, 4, (char*)0x6000); // Read font from disk
    setupScreen();
    loadFont((char*)0x6000);

    clear();
    print("kern.\n\n");

    diskRead(4, 1, filetable); // Read the filetable and store it in memory

    // Run Interactive Shell Program
    // TODO: Make it in another file
    while (1) {
        print(PROMPT);
        char* input = read('\n');
        
        if (input[0] == '\n') continue;

        char* args;
        if ((args = strchr(input, ' ')) == NULL) {
            args = strchr(input, '\n');
        }

        *args++ = '\0'; // Separate command from args

        if (strcmp(input, "clear") == 0) {
            clear();
        } else if (strcmp(input, "disk") == 0) {
            char buf[512] = { 0 };
            
            if (*args == '\0') {
                print("disk: ");
                print("No arguments given\n");
                print("Usage: disk <r/w> <sector> [data]\n");
                continue;
            }

            while (isSpace(*args)) { args++; } // Skip whitespace
            char mode = *args++;
            while (isSpace(*args)) { args++; }
            
            if (mode == 'r') {
                if (*args == '\0') {
                    print("disk: ");
                    print("Sector not specified\n");
                    print("Usage: disk <r/w> <sector> [data]\n");
                    continue;
                }

                uint32_t sector = atoi(args);
                print("Reading...\n");
                diskRead(sector, 1, buf);
                
                print("Contents:\n");
                for (size_t i = 0; i < 512; i++) {
                    if (buf[i] != '\0') {
                        putc(buf[i]);
                    }
                }

                putc('\n');
            } else if (mode == 'w') {
                char* data;
                if ((data = strchr(args, ' ')) == NULL) {
                    print("disk: ");
                    print("No data given\n");
                    print("Usage: disk <r/w> <sector> [data]\n");
                    continue;
                }

                *data++ = '\0';
                uint32_t sector = atoi(args);

                while (isSpace(*args)) { args++; } // Skip whitespace

                strcpy(buf, data);
                print("Writing...\n");
                diskWrite(sector, 1, buf);
            } else { 
                print("disk: ");
                print("Invalid access mode. Use r(ead) or w(rite)\n");
                print("Usage: disk <r/w> <sector> [data]\n");
                continue;
            }
        } else if (strcmp(input, "echo") == 0) {
            print(args);
        } else if (strcmp(input, "exit") == 0) {
            break;
        } else if (strcmp(input, "help") == 0) {
            print("Available Commands:\n");
            print(" clear      | Clears the screen\n");
            print(" disk       | Reads/Writes data from/to the disk\n");
            print(" echo       | Prints a message to stdout\n");
            print(" exit       | Exits shell\n");
            print(" help       | Prints this message\n");
            print(" ls         | Lists all available files\n");
            print(" reboot     | Reboots the system\n");
            print(" test       | Performs tests\n");
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

                if (sector < 10) putc('0');
                if (sector == 0) putc('0');
                else print(itoa(sector, 10));
                print(": ");
                print(filename);

                for (uint8_t i = 14 - strlen(filename); i > 0; i--) {
                    putc(' ');
                }

                print(" | ");
                if (size >> 1) {
                    print(itoa(size >> 1, 10));
                    if (size & 0x01) { print(".5"); }
                    print("kB\n");
                } else {
                    print(itoa(size*512, 10));
                    print("B\n");
                }
            }
        } else if (strcmp(input, "reboot") == 0) {
            outb(0x64, 0xFE);
        } else if (strcmp(input, "test") == 0) {
            print("Running Tests...\n");
            uint8_t buf[512];
            
            print("Read Test .......... ");
            diskRead(0, 1, (char*)buf);
            if (buf[510] == 0x55 && buf[511] == 0xaa) {
                print("[ DONE ]\n");
            } else {
                print("[ FAIL ]\n");
            }
            print("Write Test ......... ");
            buf[510] = 0x55; buf[511] = 0xaa;
            diskWrite(0, 1, (char*)buf);
            diskRead(0, 1, (char*)buf); // Idk how else to test if the right data was written (also could break the bootloader if it fails)
            if (buf[510] == 0x55 && buf[511] == 0xaa) {
                print("[ DONE ]\n");
            } else {
                print("[ FAIL ]\n");
            }
            print("Tests Finished!\n");
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
                            putc(buf[i]);
                        }
                    }
                    putc('\n');
                    
                    found++; // Set found to 1
                    break;
                }

                ft += 3;
            }

            if (!found) {
                print("Command not found: ");
                print(input);
                putc('\n');
            }
        }
    }
}


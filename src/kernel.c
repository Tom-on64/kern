#include <physicalMemManager.h> // TODO: Name this better
#include <disk.h>
#include <screen.h>
#include <idt.h>
#include <exceptions.h>
#include <syscall.h>
#include <pic.h>
#include <system.h>
#include <string.h>
#include <graphics.h>
#include <keyboard.h>
#include <pit.h>
#include <time.h>

#define PROMPT "#> "

char* filetable = (char*)0x7000; // Pretty sure we don't have to care about the bootloader at 0x7c00
uint32_t memmapAddr = 0x30000;

// Function declarations
void printFiletable(char* ft);
void printPhysicalMemmap();

void main() {
    // Memory manager setup
    uint32_t smapEntryCount = *(uint32_t*)0x8500;
    smapEntry_t* smapEntries = (smapEntry_t*)0x8504;
    uint32_t totalMemory = smapEntries[smapEntryCount - 1].baseAddress + smapEntries[smapEntryCount - 1].length - 1;

    setupMemoryManager(memmapAddr, totalMemory);

    for (uint32_t i = 0; i < smapEntryCount; i++) {
        if (smapEntries[i].type == 1) {
            initMemoryRegion(smapEntries[i].baseAddress, smapEntries[i].length);
        }
    }

    deinitMemoryRegion(0x1000, 0x9000); // Reserve kernel memory (under 0xa000)
    deinitMemoryRegion(memmapAddr, maxBlocks / BLOCKS_PER_BYTE);

    // TODO: Find font in filetable
    diskRead(36, 4, font); // Read font from disk
    
    clear();
    print("kern.\n\n");

    diskRead(5, 1, filetable); // Read the filetable and store it in memory
   
    // Interrupts
    setupIdt();
    setupExceptions();
    idtSetGate(0x80, syscallHandler, USR_INT_GAME_FLAGS); // Setup Syscall handler in IDT for int 0x80
    disableIrqs();
    remapPic();
    __asm__ volatile ("sti");

    setupTimer();
    setupKeyboard();

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
        } else if (strcmp(input, "gfx") == 0) {
            clear();

            // Draw some stuff :)
            for (size_t i = 0; i < 120; i++) {
                for (size_t j = 0; j < 120; j++) {
                    drawPixel(200 + i, 200 + j, GREEN);
                } 
            }

            fillRect(200, 200, 320, 320, GREEN);

            drawLine(200, 200, 320, 320, RED);
            drawLine(320, 200, 200, 320, RED);

            drawLine(800, 300, 900, 400, BLUE);
            drawLine(900, 400, 1000, 300, BLUE);
            drawLine(1000, 300, 900, 200, BLUE);
            drawLine(900, 200, 800, 300, BLUE);

            drawLine(400, 350, 420, 100, rgb(0xee, 0x9f, 0x00));

            drawTriangle(200, 400, 400, 600, 200, 600, MAGENTA);
            fillTriangle(750, 400, 800, 450, 700, 450, CYAN);
            drawTriangle(960, 400, 1000, 500, 990, 580, YELLOW);

            drawRect(100, 650, 800, 900, rgb(0xe0, 0x0a, 0x3a));

            drawCircle(1500, 600, 300, WHITE);
            fillCircle(950, 750, 75, rgb(0xa2, 0xd6, 0xf9));

            read('q'); // Wait until we get a 'q'
            clear();
        } else if (strcmp(input, "help") == 0) {
            print("Available Commands:\n");
            print(" clear      | Clears the screen\n");
            print(" disk       | Reads/Writes data from/to the disk\n");
            print(" echo       | Prints a message to stdout\n");
            print(" exit       | Exits shell\n");
            print(" gfx        | Does a graphics test\n");
            print(" help       | Prints this message\n");
            print(" ls         | Lists all available files\n");
            print(" memmap     | Prints the memory map and info\n");
            print(" reboot     | Reboots the system\n");
            print(" sleep      | Sleeps for input number of seconds\n");
            print(" test       | Performs tests\n");
        } else if (strcmp(input, "ls") == 0) {
            printFiletable(filetable);
        } else if (strcmp(input, "memmap") == 0) {
            printPhysicalMemmap();
        } else if (strcmp(input, "reboot") == 0) {
            outb(0x64, 0xFE);
        } else if (strcmp(input, "sleep") == 0) {
            if (*args == '\0') continue;

            uint32_t ms = atoi(args) * 1000; // Convert seconds to miliseconds
            if (ms == 0) continue;

            sleep(ms);
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
            
            print("\nTests Finished!\n");
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
                    found++; // Set found to 1

                    char* fileType = strchr(filename, '.') + 1;

                    uint32_t neededBlocks = (size * 512) / BLOCK_SIZE + 1;

                    /* Debug printing
                    print("Allocating ");
                    print(itoa(neededBlocks, 10));
                    print(" Block");
                    if (neededBlocks > 1) putc('s');
                    */

                    char* address = allocBlocks(neededBlocks);

                    if (address == NULL) {
                        print("\nNot enough memory to allocate!\n");
                        break;
                    }

                    /* Debug printing
                    print("\nAllocated to address 0x");
                    print(itoa((uint32_t)address, 16));
                    print("!\n");
                    */

                    diskRead(sector, size, address);

                    if (strcmp(fileType, "bin") == 0) {
                        clear(); 
                        ((void(*)(void))address)(); // Call program
                        clear();
                    } else if (strcmp(fileType, "tab") == 0) {
                        printFiletable(address);
                    } else if (strcmp(fileType, "fnt") == 0) {
                        print("Loading ");
                        print(filename);
                        print("...\n");
                        memcpy(address, font, size * 512);
                        print("Font loaded\n");
                    } else {
                        for (size_t i = 0; i < size * 512; i++) {
                            if (address[i] != '\0') {
                                putc(address[i]);
                            }
                        }
                        putc('\n');    
                    }
                    
                    // Debug printing
                    // print("Freeing allocated memory...\n");
                    freeBlocks(address, neededBlocks);
                    // print("Done!\n");

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

void printFiletable(char* ft) {
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
}

void printPhysicalMemmap() {
    uint32_t entryCount = *(uint32_t*)SMAP_ADDRESS;
    smapEntry_t* smapEntry = (smapEntry_t*)(SMAP_ADDRESS+4);

    for (uint32_t i = 0; i < entryCount; i++) {
        print("Region ");
        print(itoa(i, 10));
        print(" - base: 0x");
        print(itoa(smapEntry->baseAddress, 16));
        print(" length: 0x");
        print(itoa(smapEntry->length, 16));
        print(" type: ");
        print(itoa(smapEntry->type, 10));

        switch (smapEntry->type) {
            case 1: print(" (Available)"); break;
            case 2: print(" (Reserved)"); break;
            case 3: print(" (ACPI Reclaim)"); break;
            case 4: print(" (ACPI NVS)"); break;
            default: print(" (Reserved)"); break;
        }

        putc('\n');
        smapEntry++;
    }

    smapEntry--; // We incremented it after the last entry

    print("\nTotal memory: 0x");
    print(itoa(smapEntry->baseAddress + smapEntry->length - 1, 16));
    print(" Bytes\n");

    print("Total 4kB Blocks: ");
    print(itoa(maxBlocks, 10));
    print("\nUsed or reserved blocks: ");
    print(itoa(usedBlocks, 10));
    print("\nFree blocks: ");
    print(itoa(maxBlocks - usedBlocks, 10));
    print("\n\n");
}


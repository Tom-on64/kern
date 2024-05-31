#include <interrupt/idt.h>
#include <interrupt/exceptions.h>
#include <interrupt/syscalls.h>
#include <interrupt/pic.h>
#include <interrupt/pit.h>
#include <interrupt/rtc.h>
#include <keyboard/keyboard.h>

#include <memory/physical.h>
#include <memory/virtual.h>
#include <memory/addresses.h>
#include <memory/malloc.h>
#include <disk/filesys.h>
#include <disk/disk.h>
#include <screen/text.h>
#include <screen/graphics.h>
#include <sound/pcspk.h>
#include <sound/notes.h>
#include <ports/io.h>

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <color.h>

#define PROMPT "#> "

// Function declarations
void printFiletable(char* ft);
void printPhysicalMemmap();

__attribute__ ((section("entry")))
void main() {
    // Retrieve global variables
    currentPd = (pageDirectory_t*)*(uint32_t*)CURRENT_PAGE_DIR_ADDR; // Don't ask
    memoryMap = (uint32_t*)MEMMAP_AREA;
    maxBlocks = *(uint32_t*)PHYS_MEM_MAX_BLOCKS;
    usedBlocks = *(uint32_t*)PHYS_MEM_USED_BLOCKS;

    // Interrupt setup
    setupIdt();
    setupExceptions();
    idtSetGate(0x80, syscallHandler, USR_INT_GAME_FLAGS); // Setup Syscall handler in IDT for int 0x80
    disableIrqs();
    remapPic();
    unsetIrqMask(2); // Enable PIC 2 line
    __asm__ volatile ("sti");

    setupTimer();
    setupRtc();
    setupKeyboard();

    // Setup kernel malloc variables
    mallocNode_t* kernelMallocListHead = 0;
    uint32_t kernelMallocVirtualAddr = KERNEL_MALLOC_AREA;
    uint32_t kernelMallocPhysicalAddr = (uint32_t)allocBlocks(1); // TODO: Kernel may use more than 4kB?
    uint32_t kernelMallocPages = 1;

    mapPage((void*)kernelMallocPhysicalAddr, (void*)kernelMallocVirtualAddr);
    ptEntry_t* kernelMallocPage = getPage(kernelMallocVirtualAddr);
    SET_ATTR(kernelMallocPage, PTE_READ_WRITE);
    kernelMallocListHead = (mallocNode_t*)kernelMallocVirtualAddr;

    kernelMallocListHead->size = PAGE_SIZE - sizeof(mallocNode_t);
    kernelMallocListHead->free = true;
    kernelMallocListHead->next = NULL;

    mallocListHead = kernelMallocListHead;
    mallocVirtualAddr = kernelMallocVirtualAddr;
    mallocPhysicalAddr = kernelMallocPhysicalAddr;
    mallocPages = kernelMallocPages;
 
    // Screen setup
    loadFont("term16n.fnt");
    terminal->fg = FG_COLOR;
    terminal->bg = BG_COLOR;
    clear();
    printf("kern.\n\n");

    // Run Interactive Shell Program
    // TODO: Make it in another file
    while (1) {
        printf(PROMPT);
        char input[256];
        char* inputPtr = input;

        read(input);

        if (*input == '\0') continue;

        int argc = 0;
        char* argv[10] = { NULL };

        while (*inputPtr == ' ') inputPtr++;
        argv[argc++] = inputPtr;

        while (*inputPtr != '\0') {
            if (*inputPtr == ' ') {
                *inputPtr++ = '\0';
                while (*inputPtr == ' ') inputPtr++; // Skip extra spaces
                argv[argc++] = inputPtr;
            } else {
                inputPtr++;
            }
        }

        if (strcmp(argv[0], "clear") == 0) {
            clear();
        } else if (strcmp(argv[0], "echo") == 0) {
            if (argc < 2) continue;

            char* str = argv[1];
            char* printStr = argv[1];

            while (*argv[1] != '\0') {
                if (*argv[1] == '\\') {
                    argv[1]++;
                    if (*argv[1] == 'n') *str++ = '\n';
                    else if (*argv[1] == 'b') *str++ = '\b';
                    else if (*argv[1] == 'e') *str++ = '\x1b';
                    else if (*argv[1] == '\\') *str++ = '\\';
                    else {
                        *str++ = '\\';
                        *str++ = *argv[1];
                    }
                    argv[1]++;
                } else {
                    *str++ = *argv[1]++;
                }
            }
            *str = '\0';
            printf("%s\n", printStr);
        } else if (strcmp(argv[0], "exit") == 0) {
            break;
        } else if (strcmp(argv[0], "gfx") == 0) {
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

            getc(); // Wait until we get a 'q'
            clear();
        } else if (strcmp(argv[0], "help") == 0) {
            printf("Available Commands:\n");
            printf(" clear      | Clears the screen\n");
            printf(" echo       | Prints a message to stdout\n");
            printf(" exit       | Exits shell\n");
            printf(" gfx        | Does a graphics test\n");
            printf(" help       | Prints this message\n");
            printf(" ls         | Lists all available files\n");
            printf(" memmap     | Prints the memory map and info\n");
            printf(" reboot     | Reboots the system\n");
            printf(" sleep      | Sleeps for input number of seconds\n");
            printf(" soundtest  | Plays a tune :)\n");
            printf(" test       | Performs tests\n");
        } else if (strcmp(argv[0], "ls") == 0) {
            printFiletable(filetable);
        } else if (strcmp(argv[0], "memmap") == 0) {
            printPhysicalMemmap();
        } else if (strcmp(argv[0], "reboot") == 0) {
            outb(0x64, 0xFE);
        } else if (strcmp(argv[0], "sleep") == 0) {
            if (*argv[1] == '\0') continue;

            uint32_t ms = atoi(argv[1]) * 1000; // Convert seconds to miliseconds
            if (ms == 0) continue;

            sleep(ms);
        } else if (strcmp(argv[0], "soundtest") == 0) {
            printf("Playing...\n");
            enableSpeaker();

            /*
             * Megalovania - Toby Fox
             * |-D-D-d--A---Ab--G--F--D-F-G-| (Goofy ahh notation)
             * (The timing may be off)
             */
            for (uint8_t i = 0; i < 4; i++) {
                note_t first[4] = { D4, C4, H3, Hb3 };
                playNote(first[i],  120);
                playNote(first[i],  120);
                playNote(D5,  240);
                playNote(A4,  360);
                playNote(Ab4, 240);
                playNote(G4,  240);
                playNote(F4,  240);
                playNote(D4,  120);
                playNote(F4,  120);
                playNote(G4,  120);
            }
            rest(120);

            disableSpeaker();
        } else if (strcmp(argv[0], "test") == 0) {
            printf("\x1b[1MError: 'test' is not available.\x1b[8M");
        } else {
            fileEntry_t* file = findFile(argv[0]);
            
            if (file == NULL) {
                printf("Command not found: %s\n", argv[0]);
                continue;
            }

            uint32_t neededPages = (file->length * 512) / PAGE_SIZE;
            if ((file->length * 512) % PAGE_SIZE > 0) { neededPages++; }

            char* entryPoint = (char*)0x400000; // Right after first 4MB
            for (uint32_t i = 0; i < neededPages; i++) {
                ptEntry_t page = 0;
                uint32_t physicalAddress = (uint32_t)allocPage(&page);
                
                if (mapPage((void*)physicalAddress, (void*)(entryPoint + i * PAGE_SIZE)) != 0) {
                    printf("\n\x1b[1MNot enough memory to allocate!\n");
                    break;
                }
            }

            readFile(argv[0], entryPoint);

            if (strcmp(file->filetype, "bin") == 0) {
                // Reset malloc
                mallocListHead = NULL;
                mallocVirtualAddr = (uint32_t)entryPoint + neededPages * PAGE_SIZE;
                mallocPhysicalAddr = 0;
                mallocPages = 0;

                // Call program
                // TODO: Use return value
                ((int(*)(int argc, char** argv))entryPoint)(argc, argv);

                for (uint32_t i = 0, virtualAddr = mallocVirtualAddr; i < mallocPages; i++, virtualAddr += PAGE_SIZE) {
                    ptEntry_t* page = getPage(virtualAddr);

                    if (PAGE_PHYS_ADDRESS(page) && TEST_ATTR(page, PTE_PRESENT)) {
                        freePage(page);
                        unmapPage((void*)virtualAddr);
                        flushTlbEntry(virtualAddr);
                    }
                }
                
                mallocListHead = kernelMallocListHead;
                mallocVirtualAddr = kernelMallocVirtualAddr;
                mallocPhysicalAddr = kernelMallocPhysicalAddr;
                mallocPages = kernelMallocPages;
            } else if (strcmp(file->filetype, "tab") == 0) {
                printFiletable(entryPoint);
            } else if (strcmp(file->filetype, "fnt") == 0) {
                printf("Loading %s...\n", file->filename);
                memcpy(entryPoint, (char*)FONT_LOC, file->length * 512);
                printf("Font loaded\n");
            } else {
                for (size_t i = 0; i < file->length * 512; i++) {
                    if (entryPoint[i] != '\0') {
                        putc(entryPoint[i]);
                    }
                }
                putc('\n');    
            }

            for (uint32_t i = 0, virtualAddr = (uint32_t)entryPoint; i < neededPages; i++, virtualAddr += PAGE_SIZE) {
                ptEntry_t* page = getPage(virtualAddr);
                if (PAGE_PHYS_ADDRESS(page) && TEST_ATTR(page, PTE_PRESENT)) { // Check if the page is used
                    freePage(page);
                    unmapPage((void*)virtualAddr);
                    flushTlbEntry(virtualAddr); // Invalidate unused page
                }
            }
        }
    }

    outw(0x604, 0x2000); //  QEMU - Quit emulator
    while (1); // Just hang
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

        // TODO: Use printf() here for padding
        if (sector < 10) putc('0');
        if (sector == 0) putc('0');
        else printf("%d", sector);
        printf(": %s", filename);

        for (uint8_t i = 14 - strlen(filename); i > 0; i--) {
            printf(" ");
        }

        printf(" | ");
        if (size >> 1) {
            printf("%d%skB\n", size >> 1, (size & 0x01) ? ".5" : "");
        } else {
            printf("%dB\n", size * 512);
        }
    }
}

void printPhysicalMemmap() {
    uint32_t entryCount = *(uint32_t*)SMAP_ENTRY_COUNT;
    smapEntry_t* smapEntry = (smapEntry_t*)SMAP_ENTRIES;

    for (uint32_t i = 0; i < entryCount; i++) {
        printf("Region %d - base: 0x%x length: 0x%x type: %d ", i, (uint32_t)smapEntry->baseAddress, (uint32_t)smapEntry->length, smapEntry->type);

        switch (smapEntry->type) {
            case 1: printf("(Available)\n"); break;
            case 2: printf("(Reserved)\n"); break;
            case 3: printf("(ACPI Reclaim)\n"); break;
            case 4: printf("(ACPI NVS)\n"); break;
            default: printf("(Reserved)\n"); break;
        }

        smapEntry++;
    }

    smapEntry--; // We incremented it after the last entry

    printf("\nTotal memory: 0x%x Bytes\n", smapEntry->baseAddress + smapEntry->length - 1);

    printf("Total 4kB Blocks: %d\n", maxBlocks);
    printf("Used or reserved blocks: %d\n", usedBlocks);
    printf("Free blocks: %d\n\n", maxBlocks - usedBlocks);
}


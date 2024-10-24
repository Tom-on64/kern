#include <interrupt/isr.h>
#include <interrupt/pit.h>
#include <interrupt/rtc.h>
#include <syscalls/syscalls.h>
#include <keyboard/keyboard.h>
#include <memory/physical.h>
#include <memory/virtual.h>
#include <memory/addresses.h>
#include <memory/malloc.h>
#include <fs/fs.h>
#include <fs/impl.h>
#include <disk/disk.h>
#include <screen/text.h>
#include <screen/graphics.h>
#include <screen/gfxmode.h>
#include <tty/tty.h>
#include <sound/pcspk.h>
#include <sound/notes.h>
#include <ports/io.h>
#include <ports/serial.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <syscall.h>
#include <sys/gfx.h>

/* Little teaser
#include <math.h>
void mathTest() {
    printf("|-4| = %d\n", abs(-4));
    printf("|-3.14| = %f\n", fabs(-3.14));
    printf("e^3 = %f\n", exp(3));
    printf("e^5 = %f\n", exp2(5));
    printf("ln(2) = %f\n", log(2));
    printf("log2(69) = %f\n", log2(69));
    printf("log10(100) = %f\n", log10(100));
    printf("sqrt(2) = %f\n", sqrt(2));
    printf("cbrt(27) = %f\n", cbrt(27));
    printf("hyp(8, 6) = %f\n", hypot(8, 6));
    printf("3^4 = %f\n", pow(3, 4));
}
*/

#define PROMPT "#>"

// FS things
char* cwd;

// Function declarations
void listFiles();
void printRegs();
void printPhysicalMemmap();
void initOpenFileTable();

__attribute__ ((section("entry")))
void main() {
    // Retrieve global variables
    currentPd = (pageDirectory_t*)*(uint32_t*)CURRENT_PAGE_DIR_ADDR; // Don't ask
    memoryMap = (uint32_t*)MEMMAP_AREA;
    maxBlocks = *(uint32_t*)PHYS_MEM_MAX_BLOCKS;
    usedBlocks = *(uint32_t*)PHYS_MEM_USED_BLOCKS;

    // Setup serial output for debugf()
    if (setupSerial(PORT_COM1) != 0) { printf("\x1b[1MFailed to initialize serial interface\x1b[8M\n"); }
    else {
        serialPrint(PORT_COM1, "\x1b[H\x1b[J"); // Clear
        serialPrint(PORT_COM1, "kern. \x1b[36m(Serial console)\x1b[0m\n\n");
    }

    // Interrupt setup
    setupIsrs();
    
    // Programmable Interval Timer
    registerIrq(0, timerHandler);   
    setPitPhase(0, 2, 1000);        // 1000Hz
    
    // Real Time Clock
    registerIrq(8, rtcHandler);     
    enableRtc();

    // Keyboard
    registerIrq(1, keyboardHandler);

    // Setup kernel malloc variables
    mallocNode_t* kernelMallocListHead = 0;
    uint32_t kernelMallocVirtualAddr = KERNEL_MALLOC_AREA;
    uint32_t kernelMallocPhysicalAddr = (uint32_t)allocBlocks(16); // TODO: Kernel may use more than 64kB?
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
    
    // FS Setup
    initOpenFileTable();
    cwd = malloc(1024);
    strcpy(cwd, "/");

    // Terminal setup
    console->fg = FG_COLOR;
    console->bg = BG_COLOR;

    printf("\x1b[J"); // Clear
    printf("kern.\n\n");

    while (1) {
        printf("\x1b[6M%s \x1b[2M%s \x1b[8M", cwd, PROMPT);
        char input[256];
        char* inPtr = &input[0];

        reads(inPtr);
    }
}

int _m() {
    // Run Interactive Shell Program
    // TODO: Make it in another file
    while (1) {
        printf("\x1b[6M%s \x1b[2M%s ", cwd, PROMPT);
        char input[256];
        char* inputPtr = input;

        reads(input);

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
            printf("\x1b[J");
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
            printf("\x1b[J");

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

            getc(); // Wait until we get a keystroke
            printf("\x1b[J");
        } else if (strcmp(argv[0], "help") == 0) {
            printf("Available Commands:\n");
            printf(" clear      | Clears the screen\n");
            printf(" echo       | Prints a message to stdout\n");
            printf(" exit       | Exits shell\n");
            printf(" gfx        | Does a graphics test\n");
            printf(" help       | Prints this message\n");
            printf(" ls         | Lists files in current directory\n");
            printf(" memmap     | Prints the memory map and info\n");
            printf(" reboot     | Reboots the system\n");
            printf(" regs       | Prints register values\n");
            printf(" sleep      | Sleeps for input number of seconds\n");
            printf(" soundtest  | Plays a tune :)\n");
        } else if (strcmp(argv[0], "ls") == 0) {
            listFiles();
        } else if (strcmp(argv[0], "memmap") == 0) {
            printPhysicalMemmap();
        } else if (strcmp(argv[0], "reboot") == 0) {
            outb(0x64, 0xFE);
        } else if (strcmp(argv[0], "regs") == 0) {
            printRegs();
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
        } else { // TODO: Use cwd for this
            char* fileName = argv[0];
            
            FILE* _f = fopen(fileName, "rb");
            if (_f == NULL) {
                printf("Command not found: %s\n", fileName);
                continue;
            }
            FILE* fp = fsInfo->openFileTable + _f->_file;

            printf("%d: 0x%x Bytes\n", fp->_file, fp->_size);
            for (size_t i = 0; i < fp->_size; i++) {
                if (i % 32 == 0) {
                    if (i != 0) {
                        printf("    ");
                        size_t oldi = i;
                        i -= 32;
                        while (i < oldi) {
                            char c = fp->_ptr[i++];
                            printf("%c", c < 32 ? '.' : c);
                        }
                        putc('\n');
                    }
                    printf("%04x: ", i);
                }
                printf("%02x ", fp->_ptr[i]);
            }
            if (fp->_size % 32 != 0) {
                for (size_t i = 0; i < 32-(fp->_size % 32); i++) {
                    printf("   ");
                }
            }
            printf("    ");
            for (size_t i = fp->_size-(fp->_size%32); i < fp->_size; i++) {
                char c = fp->_ptr[i];
                printf("%c", c < 32 ? '.' : c);
            }
            printf("\n");

            free(_f);
            fclose(fp);
        }
    }

    outw(0x604, 0x2000); //  QEMU - Quit emulator
    while (1); // Just hang
}

void listFiles() {
    inode_t* root = (inode_t*)impl_superblock->rootInodePtr;
    diskRead(root->extent[0].block * 8, root->extent[0].length * 8, (void*)SCRATCH_BLOCK_LOC);
    dirEntry_t* dentry = (dirEntry_t*)SCRATCH_BLOCK_LOC;

    uint32_t total = 0;
    while ((dentry + total)->name[0] != '\0') { total++; }

    putc(' '); // TODO
    printf("\bTotal %d\n", total);

    while (dentry->name[0] != '\0') {
        inode_t* inode = (inode_t*)BOOT_FIRST_INODE_LOC + dentry->id;

        printf("%s", dentry->name);
        for (uint32_t pad = (16 - strlen(dentry->name)); pad > 0; pad--) { putc(' '); }

        switch (inode->type) {
            case FT_FILE: printf("%dB\n", inode->sizeBytes); break;
            case FT_DIR: printf("[dir]\n"); break;
        }

        dentry++;
    }
}

void printRegs() {
    uint32_t val;

    // a, b, c, d
    __asm__ volatile ("movl %%eax, %0" : "=r"(val));
    printf("eax     0x%08x\n", val);
    __asm__ volatile ("movl %%ebx, %0" : "=r"(val));
    printf("ebx     0x%08x\n", val);
    __asm__ volatile ("movl %%ecx, %0" : "=r"(val));
    printf("ecx     0x%08x\n", val);
    __asm__ volatile ("movl %%edx, %0" : "=r"(val));
    printf("edx     0x%08x\n", val);

    // si, di
    __asm__ volatile ("movl %%esi, %0" : "=r"(val));
    printf("esi     0x%08x\n", val);
    __asm__ volatile ("movl %%edi, %0" : "=r"(val));
    printf("edi     0x%08x\n", val);

    // cs, ds, es, ss
    __asm__ volatile ("movl %%cs, %0" : "=r"(val));
    printf("cs      0x%08x\n", val);
   __asm__ volatile ("movl %%ds, %0" : "=r"(val));
    printf("ds      0x%08x\n", val);
    __asm__ volatile ("movl %%es, %0" : "=r"(val));
    printf("es      0x%08x\n", val);
    __asm__ volatile ("movl %%ss, %0" : "=r"(val));
    printf("ss      0x%08x\n", val);

    // stack
    __asm__ volatile ("movl %%esp, %0" : "=r"(val));
    printf("esp     0x%08x\n", val);
    __asm__ volatile ("movl %%ebp, %0" : "=r"(val));
    printf("ebp     0x%08x\n", val);
}

void printPhysicalMemmap() {
    uint32_t entryCount = *(uint32_t*)SMAP_ENTRY_COUNT;
    smapEntry_t* smapEntry = (smapEntry_t*)SMAP_ENTRIES;

    for (uint32_t i = 0; i < entryCount; i++) {
        printf("Region %02d - base: 0x%08x length: 0x%08x type: %2d ", i, (uint32_t)smapEntry->baseAddress, (uint32_t)smapEntry->length, smapEntry->type);

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


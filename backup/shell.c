#include <stdlib.h>
#include <stdio.h>
#include "shell.h"

#define COMMAND_COUNT   11
#define MAX_INPUT       256
#define MAX_ARGC        9       // 10 - 1 (because 0 is also an arg)

typedef struct {
    char* command;
    int (*handler)(int argc, char** argv);
    char* help;
} command_t;

command_t commands[COMMAND_COUNT] = {
    { "clear",      cmd_clear,      "Clears the screen" },
    { "echo",       cmd_echo,       "Prints a message to stdout" },
    { "exit",       cmd_exit,       "Exits shell" },
    { "gfxtest",    cmd_gfxtest,    "Does a graphics test" },
    { "help",       cmd_help,       "Prints this message" },
    { "ls",         cmd_ls,         "Lists files in current directory" },
    { "memmap",     cmd_memmap,     "Prints the memory map and info" },
    { "reboot",     cmd_reboot,     "Reboots the system" },
    { "regs",       cmd_regs,       "Prints register values" },
    { "sleep",      cmd_sleep,      "Sleeps for input number of seconds" },
    { "soundtest",  cmd_soundtest,  "Plays a tune :)" },
};

int prompt() {
    printf("\x1b[6M%s \x1b[2M%s ", cwd, PROMPT);
    char input[256];
    gets(input, 256);

    // Empty line
    if (*input == '\0') return 0;

    // Parse argc & argv
    char* inptr = input;
    int argc;
    char* argv[10] = { NULL };

    
    while (*inptr == ' ') inptr++;
    argv[argc++] = inptr;

    while (*inptr != '\0') {
        if (*inptr == ' ') {
            *inptr++ = '\0';
            while (*inptr == ' ') inptr++; // Skip extra spaces
            argv[argc++] = inptr;
        } else {
            inptr++;
        }
    }

    return eval(argc, argv);
}

int eval(int argc, char** argv) {
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            return commands[i].handler(argc, argv);
        }
    }

    // TODO: read PATH
    return 1;
}

int cmd_clear(int argc, char** argv) { 
    printf("\x1b[J");
    return 0;
}

int cmd_echo(int argc, char** argv) { 
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

    return 0;
}

int cmd_exit(int argc, char** argv) { 
    outw(0x604, 0x2000); //  QEMU - Quit emulator
    while (1) { __asm__ volatile ("cli; hlt"); }
    
    return 1;   // Unreachable
}

int cmd_gfxtest(int argc, char** argv) {
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
    getc(stdin); // Wait until we get a keystroke
    printf("\x1b[J");

    return 0;
}

int cmd_help(int argc, char** argv) { 
    printf("Available commands:\n");
    for (int i = 0; i < COMMAND_COUNT; i++) {
        printf(" %11s | %s\n", commands[i].command, commands[i].help);
    }

    return 0;
}

int cmd_ls(int argc, char** argv) {
    inode_t* root = (inode_t*)impl_superblock->rootInodePtr;
    diskRead(root->extent[0].block * 8, root->extent[0].length * 8, (void*)SCRATCH_BLOCK_LOC);
    dirEntry_t* dentry = (dirEntry_t*)SCRATCH_BLOCK_LOC;

    uint32_t total = 0;
    while ((dentry + total)->name[0] != '\0') { total++; }

    putc(' ', stdout); // TODO
    printf("\bTotal %d\n", total);

    while (dentry->name[0] != '\0') {
        inode_t* inode = (inode_t*)BOOT_FIRST_INODE_LOC + dentry->id;

        printf("%s", dentry->name);
        for (uint32_t pad = (16 - strlen(dentry->name)); pad > 0; pad--) { putc(' ', stdout); }

        switch (inode->type) {
            case FT_FILE: printf("%dB\n", inode->sizeBytes); break;
            case FT_DIR: printf("[dir]\n"); break;
        }

        dentry++;
    }

    return 0;
}

int cmd_memmap(int argc, char** argv) {
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

    return 0;
}

int cmd_reboot(int argc, char** argv) { 
    outb(0x64, 0xFE);
    return 1;
}

int cmd_regs(int argc, char** argv) {
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

    return 0;
}

int cmd_sleep(int argc, char** argv) {
    if (*argv[1] == '\0') continue;
    uint32_t ms = atoi(argv[1]) * 1000; // Convert seconds to miliseconds
    if (ms == 0) continue;
    msleep(ms);

    return 0;
}

int cmd_soundtest(int argc, char** argv) {
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
    return 0;
}

_m() {
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
                putc('\n', stdout);
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


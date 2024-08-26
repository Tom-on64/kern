#ifndef INTERRUPT_SYSCALLS_H
#define INTERRUPT_SYSCALLS_H

#include <interrupt/idt.h>
#include <terminal/terminal.h>
#include <keyboard/keyboard.h>
#include <memory/malloc.h>
#include <memory/addresses.h>
#include <memory/physical.h>
#include <fs/fs.h>
#include <fs/impl.h>
#include <error/bigerr.h>
#include <syscall.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * XXX WARNING - Update MAX_SYSCALLS in the handler XXX
 */

typedef struct {
    uint32_t esp;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t ds;
    uint32_t es;
    uint32_t fs;
    uint32_t gs;
    uint32_t eax;
} __attribute__ ((packed)) sys_regs_t;

int sys_reserved(sys_regs_t) { 
    printf("\e[1N\e[6M Illegal use of Reserved() system call! \e[8M\e[8N\n");
    return -1;
}

// Args: ebx - number of miliseconds
int sys_sleep(sys_regs_t regs) {
    uint32_t* sleepTimerTicks = (uint32_t*)SLEEP_TIMER;
    *sleepTimerTicks = regs.ebx;
    
    while (*sleepTimerTicks > 0) { // Wait...
        __asm__ volatile ("sti;hlt;cli;");
    }

    return 0;
}

// Args: ebx - int fd
//       ecx - void* buf
//       edx - uint32_t len
// Rets: Number of bytes written
int sys_write(sys_regs_t regs) {
    int32_t fd = regs.ebx;
    void* buf = (void*)regs.ecx;
    uint32_t len = regs.edx;

    // TODO: Open fd table?
    if (fd == 1) { // "stdout"
        return terminalWrite(buf, len); // Call a terminal print function
    } else { 
        return -1;
    }
}

// TODO: Use a Read() sycall and read from stdin
// Args: ebx - char* buffer
int sys_read(sys_regs_t regs) {
    char* s = (char*)regs.ebx;
    reads(s);

    return 0;
}

// Args: ebx - size_t count
// Rets: Pointer to malloc-ed memory
int sys_malloc(sys_regs_t regs) {
    uint32_t count = regs.ebx;
    
    // First Malloc()
    if (!mallocListHead) {
        mallocInit(count); // Setup malloc linked list
    }

    void* ptr = mallocNextNode(count);
    mallocMergeFree();

    return (uint32_t)ptr;
}

// Args: ebx - void* ptr
int sys_free(sys_regs_t regs) {
    void* ptr = (void*)regs.ebx;
    mallocFree(ptr);
    return 0;
}

// Args: ebx - path, 
//       ecx - oflag
// Rets: Opened file descriptor
int sys_open(sys_regs_t regs) {
    int fd = 0;
    char* path = (char*)regs.ebx;
    uint32_t oflag = regs.ecx;

    extern FILE* openFileTable;
    extern uint32_t openFiles;

    // TODO: better file finding
    inode_t* root = (inode_t*)impl_superblock->rootInodePtr;
    loadFile(root, (void*)SCRATCH_BLOCK_LOC);

    inode_t* inode = getInode(path, (dirEntry_t*)SCRATCH_BLOCK_LOC);
    if (inode == NULL) {
        return -1;
    }

    // TODO: check if max open files have'nt been reached
    fd = openFiles++;
    FILE* fp = openFileTable + fd;
    fp->_ptr = (void*)0xd000;
    fp->_offset = 0;
    fp->_size = inode->sizeBytes;
    fp->_flag = oflag;
    fp->_file = fd;

    loadFile(inode, fp->_ptr);

    return fd;
}

// Args: ebx - int fd
// Rets: status
int sys_close(sys_regs_t regs) {
    int fd = regs.ebx;

    extern FILE* openFileTable;
    extern uint32_t openFiles;

    FILE* fp = openFileTable + fd;
    free(fp->_ptr);
    openFiles--;

    return 0;
}

// TODO
int sys_seek(sys_regs_t regs) { return -1; }

// System call table
int (*syscalls[MAX_SYSCALLS])(sys_regs_t) = {
    [SYS_RESERVED]  = sys_reserved,
    [SYS_SLEEP]     = sys_sleep,  
    [SYS_WRITE]     = sys_write,  
    [SYS_READ]      = sys_read,   
    [SYS_MALLOC]    = sys_malloc, 
    [SYS_FREE]      = sys_free,
    [SYS_OPEN]      = sys_open,
    [SYS_CLOSE]     = sys_close,
    [SYS_SEEK]      = sys_seek,
};

// int 0x80 - Syscall interrupt, handled by this function
// eax : Syscall number
__attribute__ ((naked)) // No prologue or epilogue, just assembly
void syscallHandler(intFrame_t* iframe) {
    // Interrupt - Stack should contain: ss, esp, eflags, cs, eip
    // We should also push these regs: ax, gs, fs, es, ds, bp, di, si, dx, cx, bx
    __asm__ volatile (
            ".intel_syntax noprefix\n" // We use intel syntax
            ".equ MAX_SYSCALLS, 9\n" // Define MAX_SYSCALLS again TODO: I don't want to have to do this

            // Check if syscall exists
            "cmp eax, MAX_SYSCALLS - 1\n"
            "ja invalidSyscall\n"

            // Call the syscall
            "push eax\n"
            "push gs\n"
            "push fs\n"
            "push es\n"
            "push ds\n"
            "push ebp\n"
            "push edi\n"
            "push esi\n"
            "push edx\n"
            "push ecx\n"
            "push ebx\n"
            "push esp\n" // Push stack pointer again (for variables and such idk)

            "call [syscalls + eax * 4]\n" // Call syscall from table. Syscall number should be in eax
            "add esp, 4\n" // Do not override ESP
            "pop ebx\n"
            "pop ecx\n"
            "add esp, 4\n" // Do not override EDX, some syscalls use it

            "pop esi\n"
            "pop edi\n"
            "pop ebp\n"
            "pop ds\n"
            "pop es\n"
            "pop fs\n"
            "pop gs\n"
            "add esp, 4\n" // Save eax value
            "iretd\n" // Return from interrupt (32 bit)

            // TODO: Send an error or something here
        "invalidSyscall:\n"
            "mov eax, -1\n" // Current error thing
            "iretd\n"

            ".att_syntax" // Switch back to AT&T Syntax (Just in case)
    );
}

#endif

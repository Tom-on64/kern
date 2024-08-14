#ifndef SYSCALL_H
#define SYSCALL_H

#include <interrupt/idt.h>
#include <terminal/terminal.h>
#include <keyboard/keyboard.h>
#include <memory/malloc.h>
#include <memory/addresses.h>
#include <syscall.h>
#include <stdint.h>
#include <fs/fs.h>

/*
 * XXX WARNING - Update MAX_SYSCALLS in the handler XXX
 */

// Args: ebx - number of miliseconds
void sys_sleep() {
    uint32_t* sleepTimerTicks = (uint32_t*)SLEEP_TIMER;
    
    __asm__ volatile ("movl %%ebx, %0" : "=r"(*sleepTimerTicks));

    while (*sleepTimerTicks > 0) { // Wait...
        __asm__ volatile ("sti;hlt;cli;");
    }
}

// Args: ebx - int fd
//       ecx - void* buf
//       edx - uint32_t len
// Rets: Number of bytes written
void sys_write() {
    int32_t fd = 0;
    void* buf = NULL;
    uint32_t len = 0;

    __asm__ volatile ("nop" : "=b"(fd), "=c"(buf), "=d"(len));

    // TODO: Open fd table?
    if (fd == 1) { // "stdout"
        int32_t result = terminalWrite(buf, len); // Call a terminal print function
        __asm__ volatile ("movl %0, %%eax" : : "r"(result));
    } else { // Not implemented
        __asm__ volatile ("movl $-1, %eax");
    }
}

// TODO: Use a Read() sycall and read from stdin
// Args: ebx - char* buffer
void sys_gets() {
    char* s;
    __asm__ volatile ("movl %%ebx, %0" : "=r"(s));
    reads(s);
}

// Args: ebx - size_t count
// Rets: Pointer to malloc-ed memory
void sys_malloc() {
    uint32_t count = 0;
    __asm__ volatile ("movl %%ebx, %0" : "=b"(count));
    
    // First Malloc()
    if (!mallocListHead) {
        mallocInit(count); // Setup malloc linked list
    }

    void* ptr = mallocNextNode(count);
    mallocMergeFree();

    // Return allocated address in eax
    __asm__ volatile ("movl %0, %%eax" : : "r"(ptr));
}

// Args: ebx - void* ptr
void sys_free() {
    void* ptr = NULL;
    __asm__ volatile ("movl %%ebx, %0" : "=b"(ptr));
    mallocFree(ptr);
}

// Args: ebx - path, 
//       ecx - oflag
// Rets: Opened file descriptor
void sys_open() {
    uint32_t fd = 0;
    char* path = NULL;
    uint32_t oflag = 0;

    __asm__ volatile ("nop" : "=b"(path), "=c"(oflag));
    
    __asm__ volatile ("movl %0, %%eax" : : "a"(fd));
}

// Args: ebx - int fd
// Rets: status
void sys_close() {}

// TODO
void sys_seek() {}

// System call table
void (*syscalls[MAX_SYSCALLS])(void) = {
    [SYS_SLEEP]     = sys_sleep,  
    [SYS_WRITE]     = sys_write,  
    [SYS_GETS]      = sys_gets,   
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
            "push eax\n" // Push required values
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
            "add esp, 4\n"
            "pop ebx\n" // Pop pushed values
            "pop ecx\n"
            "pop edx\n"
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

            ".att_syntax" // Switch back to Att Syntax (Just in case)
    );
}

#endif

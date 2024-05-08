#ifndef SYSCALL_H
#define SYSCALL_H

#include <interrupt/pit.h>
#include <screen/text.h>

// System call count
#define MAX_SYSCALLS 2

// Args: ebx - number of miliseconds
void sys_sleep() {
    uint32_t ms;
    __asm__ volatile ("movl %%ebx, %0" : "=r"(ms));

    uint32_t requiredTicks = *timerTicks + ms;
    while (*timerTicks != requiredTicks) { // Wait...
        __asm__ volatile ("sti;hlt;cli;");
    }
}

// TODO: Use a Write() syscall and write to stdout
// Args: ebx - string pointer
void sys_puts() {
    char* s;
    __asm__ volatile ("movl %%ebx, %0" : "=r"(s));

    print(s);
}

// System call table
void* syscalls[MAX_SYSCALLS] = {
    sys_sleep, 
    sys_puts, 
};

// int 0x80 - Syscall interrupt, handled by this function
// eax : Syscall number
__attribute__ ((naked)) // No prologue or epilogue, just assembly
void syscallHandler(void) {
    // Interrupt - Stack should contain: ss, esp, eflags, cs, eip
    // We should also push these regs: ax, gs, fs, es, ds, bp, di, si, dx, cx, bx
    __asm__ volatile (
            ".intel_syntax noprefix\n" // We use intel syntax
            ".equ MAX_SYSCALLS, 2\n" // Define MAX_SYSCALLS again

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
            "iretd\n" // Return from interrupt

            // TODO: Send an error or something here
            "invalidSyscall:\n"
            "iretd\n"

            ".att_syntax" // Switch back to Att Syntax (Just in case)
    );
}

#endif

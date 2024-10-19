#ifndef _SETJMP_H
#define _SETJMP_H

typedef struct {
    void* _bp;  // Base pointer
    void* _sp;  // Stack pointer
    void* _pc;  // Program counter
} jmp_buf;

int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int value);

#endif

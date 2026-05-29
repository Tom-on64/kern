#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

#include "isr.h"

#define EXCEPTION_DE 0
#define EXCEPTION_BP 3
#define EXCEPTION_OF 4
#define EXCEPTION_BR 5
#define EXCEPTION_UD 6
#define EXCEPTION_NM 7
#define EXCEPTION_DF 8
#define EXCEPTION_GP 13
#define EXCEPTION_PF 14
#define EXCEPTION_AC 17

typedef void (*exception_handler_ptr)(struct isr_int_frame* iframe, bool user);

const extern char* exception_strings[];

int  exception_init(void);
void exception_handle(struct isr_int_frame* iframe);

#endif

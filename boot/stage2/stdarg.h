#ifndef _STDARG_H
#define _STDARG_H

typedef char* va_list;
#define va_start(ap, paramn) (void)((ap) = (char*)(&(paramn) + 1))
#define va_end(ap) (void)((ap) = 0)
#define va_arg(ap, type) (((type*)((ap) = ((ap) + sizeof(type))))[-1])

#endif


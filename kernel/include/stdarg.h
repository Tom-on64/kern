#ifndef _STDARG_H
#define _STDARG_H

typedef __builtin_va_list va_list;
#define va_start(_ap, _last) __builtin_va_start(_ap, _last)
#define va_end(_ap)         __builtin_va_end(_ap)
#define va_arg(_ap, _type)   __builtin_va_arg(_ap, _type)

#endif

#ifndef _KERNEL_H
#define _KERNEL_H

#include <stdarg.h>
#include <types.h>

#define __alias(symbol)	__attribute__((alias(#symbol)))
#define __align(x)	__attribute__((aligned(x)))
#define __printf(a, b)	__attribute__((format(printf, a, b)))
#define __scanf(a, b)	__attribute__((format(scanf, a, b)))
#define __align_largest	__attribute__((aligned))
#define __depricated	__attribute__((depricated))
#define __packed	__attribute__((packed))
#define __noreturn	__attribute__((noreturn))
#define __const		__attribute__((const))
#define __pure		__attribute__((pure))
#define __section(_s)	__attribute__((section(_s)))
#define __cold		__attribute__((cold))
#define __hot		__attribute__((hot))
#define __naked		__attribute__((naked))
#define __always_inline	__attribute__((always_inline))
#define noinline	__attribute__((never_inline))

#define ALIGN(_addr, _align)   (((_addr) + (_align) - 1) & ~((_align) - 1))
#define ALIGN_DOWN(_addr, _align) ((_addr) & ~((_align) - 1))
#define BIT(_n)                  (1ULL << (_n))

#define swap(_a, _b)	({ typeof(_a) _tmp = (_a); (_a) = (_b); (_b) = (_a); })
#define min(_x, _y)	({ typeof(_x) __x = (_x); typeof(_y) __y = (_y); __x < __y ? __x : __y; })
#define max(_x, _y)	({ typeof(_x) __x = (_x); typeof(_y) __y = (_y); __x > __y ? __x : __y; })
#define ceil(_x, _y)	((((_x) + (_y)) (_y)) * (_y))
#define floor(_x, _y)	((_x) - ((_x) % (_y)))
#define dceil(_x, _y)	(((_x) + (_y) - 1) / (_y))

#define fence() __asm__ volatile ("" ::: "memory")
 
#define KERN_SOH	"\001"
#define KERN_SOH_ASCII	'\001'
#define KERN_EMERG	KERN_SOH "0"
#define KERN_ALERT	KERN_SOH "1"
#define KERN_CRIT	KERN_SOH "2"
#define KERN_ERROR	KERN_SOH "3"
#define KERN_WARNING	KERN_SOH "4"
#define KERN_NOTICE	KERN_SOH "5"
#define KERN_INFO	KERN_SOH "6"
#define KERN_DEBUG	KERN_SOH "7"
#define KERN_DEFAULT	""

#define LOGLEVEL_EMERG	 0
#define LOGLEVEL_ALERT	 1
#define LOGLEVEL_CRIT	 2
#define LOGLEVEL_ERROR	 3
#define LOGLEVEL_WARNING 4
#define LOGLEVEL_NOTICE	 5
#define LOGLEVEL_INFO	 6
#define LOGLEVEL_DEBUG	 7

#ifndef LOGLEVEL_DEFAULT
#define LOGLEVEL_DEFAULT LOGLEVEL_ERROR
#endif

#ifndef CONSOLE_LOGLEVEL
#define CONSOLE_LOGLEVEL 4
#endif

extern uint8_t console_loglevel;

// TODO: Add a ring buffer for the logs
int printk(const char* fmt, ...) __printf(1, 2);
int vprintk(const char* fmt, va_list ap);

#define pr_emerg(fmt, ...)	printk(KERN_EMERG fmt, ##__VA_ARGS__)
#define pr_alert(fmt, ...)	printk(KERN_ALERT fmt, ##__VA_ARGS__)
#define pr_crit(fmt, ...)	printk(KERN_CRIT fmt, ##__VA_ARGS__)
#define pr_error(fmt, ...)	printk(KERN_ERROR fmt, ##__VA_ARGS__)
#define pr_warn(fmt, ...)	printk(KERN_WARNING fmt, ##__VA_ARGS__)
#define pr_notice(fmt, ...)	printk(KERN_NOTICE fmt, ##__VA_ARGS__)
#define pr_info(fmt, ...)	printk(KERN_INFO fmt, ##__VA_ARGS__)
#define pr_debug(fmt, ...)	printk(KERN_DEBUG fmt, ##__VA_ARGS__)

#define PANIC_MSG_BUFSZ	1024

struct isr_int_frame;
void panic(struct isr_int_frame* regs, const char* fmt, ...) __noreturn __cold __printf(2, 3);
void vpanic(struct isr_int_frame* regs, const char *fmt, va_list ap) __noreturn __cold;

extern uintptr_t __kernel_start, __kernel_text_start, __kernel_rodata_start, __kernel_data_start;
extern uintptr_t __kernel_end, __kernel_text_end, __kernel_rodata_end, __kernel_data_end;

#endif

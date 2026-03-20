#ifndef _KERNEL_H
#define _KERNEL_H

// Null definition
#define NULL ((void*)0)

/*
 * Virtual memory addresses
 */
#define USER_HEAP	 0x10000000
#define USER_SHARED	 0xA0000000
#define USER_STACK	 0xBF000000
#define KERNEL_BASE	 0xC0000000
#define KERNEL_SIZE	 0x10000000
#define KERNEL_HEAP_BASE 0xD0000000
#define KERNEL_HEAP_SIZE 0x10000000
#define KERNEL_SHARED	 0xF0000000
#define KERNEL_FRAMEBUF	 0xC8000000

/*
 * Variable arguments
 */
//typedef struct { unsigned int gp_off; unsigned int fp_off; void *arg_overflow; void *reg_save; } va_list;
typedef __builtin_va_list va_list;
#define va_start(_ap, _last) __builtin_va_start(_ap, _last)
#define va_end(_ap)         __builtin_va_end(_ap)
#define va_arg(_ap, _type)   __builtin_va_arg(_ap, _type)

/*
 * Utility macros
 */

// Assertions
#define STATIC_ASSERT(_cond, _msg)\
	typedef int static_assert_##_msg[(_cond) ? 1 : -1]

// Array
#define ARRAY_SIZE(_a)	(sizeof((_a)) / sizeof((_a)[0]))
#define array_sizeof	ARRAY_SIZE
#define container_of(_p, _t, _m) \
	((_t*)((char*)(_p) - offsetof(_t, _m)))

// Align
#define ALIGN(x, a)              __ALIGN_MASK((x), (typeof(x))(a) - 1)
#define ALIGN_DOWN(x, a)         ((x) & ~((typeof(x))(a) - 1))
#define __ALIGN_MASK(x, mask)    (((x) + (mask)) & ~(mask))
#define PTR_ALIGN(p, a)          ((typeof(p))ALIGN((unsigned long)(p), (a)))
#define PTR_ALIGN_DOWN(p, a)     ((typeof(p))ALIGN_DOWN((unsigned long)(p), (a)))

// Bit
#define BIT(_n)		(1UL << (_n))
#define BIT_MASK(_n)	(BIT(_n) - 1)

// Math
#define swap(_a, _b)	({ typeof(_a) _tmp = (_a); (_a) = (_b); (_b) = (_a); })
#define min(_x, _y)	({ typeof(_x) __x = (_x); typeof(_y) __y = (_y); __x < __y ? __x : __y; })
#define max(_x, _y)	({ typeof(_x) __x = (_x); typeof(_y) __y = (_y); __x > __y ? __x : __y; })
#define ceil(_x, _y)	((((_x) + (_y)) (_y)) * (_y))
#define floor(_x, _y)	((_x) - ((_x) % (_y)))
#define dceil(_x, _y)	(((_x) + (_y) - 1) / (_y))

// Misc
#define fence()		__asm__ volatile ("":::"memory")

/*
 * Attribute definitions
 */ 
#define __align(_b)	__attribute__((aligned(_b)))
#define __depricated	__attribute__((depricated))
#define __packed	__attribute__((packed))
#define __noreturn	__attribute__((noreturn))
#define __const		__attribute__((const))
#define __pure		__attribute__((pure))
#define __inline	__attribute__((always_inline))
#define __no_inline	__attribute__((never_inline))
#define __section(_s)	__attribute__((section(_s)))
#define __cold		__attribute__((cold))
#define __hot		__attribute__((hot))
#define __naked		__attribute__((naked))

/*
 * Standard int types
 */
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned int	uint32_t;
typedef unsigned long	uint64_t;
typedef signed char	int8_t;
typedef signed short	int16_t;
typedef signed int	int32_t;
typedef signed long	int64_t;

typedef uint64_t	size_t;
typedef int64_t		ssize_t;

typedef uint64_t	uintptr_t;
typedef int64_t		intptr_t;

typedef uint64_t	offset_t;

/*
 * Boolean
 */
#define bool _Bool
#define true 1
#define false 0

/*
 * System types
 */
typedef uint32_t	pid_t;

/*
 * Linker defined values
 */
extern uint64_t __kernel_start, __kernel_text_start, __kernel_rodata_start, __kernel_data_start;
extern uint64_t __kernel_end, __kernel_text_end, __kernel_rodata_end, __kernel_data_end;

/*
 * Logging and errors
 * TODO: Use something other than debugf()
 */
#include <serial.h>
#define printk		debugf
#define pr_dbg(_s)	printk("[debug] %s\n", (_s))
#define pr_wrn(_s)	printk("[warning] %s\n", (_s))
#define pr_err(_s)	printk("[error] %s\n", (_s))
#define panic(_s)	do {\
		printk("[kernel] Panic! %s", (_s));\
		while (1) __asm__ volatile ("cli; hlt");\
	} while (0)
#define BUG()		panic("BUG()")
#define WARN()		pr_wrn("WARN()")
#define BUG_ON(_e)	((_e) ? BUG() : NULL)
#define WARN_ON(_e)	((_e) ? WARN() : NULL)

#endif

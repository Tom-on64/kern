#ifndef _KERNEL_H
#define _KERNEL_H

// Null definition
#define NULL	((void*)0)

/*
 * Variable arguments
 */
typedef void* va_list;
#define va_start(ap, param) (void)((ap) = (char*)(&(param) + 1))
#define va_end(ap) (void)((ap) = 0)
#define va_arg(ap, type) (((type*)((ap) = ((ap) + sizeof(type))))[-1])

/*
 * Utility macros
 */
#define ARRAY_SIZE(_a)	(sizeof((_a)) / sizeof((_a)[0]))
#define BIT(_n)		(1UL << (_n))
#define BIT_MASK(_n)	(BIT(_n) - 1)
#define ALIGN(_x, _a)	__ALIGN_MASK((_x), (typeof(_x))(_a) - 1)
#define __ALIGN_MASK(_x, _m) (((_x) + (_m)) & ~(_m))
#define container_of(_p, _t, _m) \
	((_t*)((char*)(_p) - offsetof(_t, _m)))
#define swap(_a, _b)	({ typeof(_a) _tmp = (_a); (_a) = (_b); (_b) = (_a); })
#define min(_x, _y)	({ typeof(_x) __x = (_x); typeof(_y) __y = (_y); __x < __y ? __x : __y; })
#define max(_x, _y)	({ typeof(_x) __x = (_x); typeof(_y) __y = (_y); __x > __y ? __x : __y; })
#define ceil(_x, _y)	((((_x) + (_y)) (_y)) * (_y))
#define floor(_x, _y)	((_x) - ((_x) % (_y)))

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
typedef uint32_t	size_t;
typedef int32_t		ssize_t;
typedef uint32_t	uintptr_t;
typedef int32_t		intptr_t;

/*
 * Logging and errors
 */
// TODO: Logging with printk()
// TODO: Log levels & pr_*() wrappers for printk()
#define BUG()		(panic("BUG()"))
#define WARN()		NULL
#define BUG_ON(_e)	((_e) ? BUG() : NULL)
#define WARN_ON(_e)	((_e) ? WARN() : NULL)
// TODO: Write a panic()
#define panic(_s)	while(1);

#endif

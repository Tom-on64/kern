#ifndef _KERNEL_H
#define _KERNEL_H

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
// Unsigned
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned int	uint32_t;
typedef unsigned long	uint64_t;
// Signed
typedef signed char	int8_t;
typedef signed short	int16_t;
typedef signed int	int32_t;
typedef signed long	int64_t;
// Size
typedef uint32_t	size_t;
typedef int32_t		ssize_t;

#endif

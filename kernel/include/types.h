#ifndef _TYPES_H
#define _TYPES_H

// NULL pointer
#define NULL ((void*)0)

/*
 * Integral types
 */
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned int	uint32_t;
typedef unsigned long	uint64_t;
typedef signed char	int8_t;
typedef signed short	int16_t;
typedef signed int	int32_t;
typedef signed long	int64_t;
_Static_assert(sizeof(int8_t)   == 1, "int8_t not 8 bits.");
_Static_assert(sizeof(int16_t)  == 2, "int16_t not 16 bits.");
_Static_assert(sizeof(int32_t)  == 4, "int32_t not 32 bits.");
_Static_assert(sizeof(int64_t)  == 8, "int64_t not 64 bits.");
_Static_assert(sizeof(uint8_t)  == 1, "uint8_t not 8 bits.");
_Static_assert(sizeof(uint16_t) == 2, "uint16_t not 16 bits.");
_Static_assert(sizeof(uint32_t) == 4, "uint32_t not 32 bits.");
_Static_assert(sizeof(uint64_t) == 8, "uint64_t not 64 bits.");

// TODO: Add a static assertion here
typedef uint64_t	size_t;
typedef int64_t		ssize_t;
typedef size_t		offset_t;

typedef uint64_t	uintptr_t;
typedef int64_t		intptr_t;
_Static_assert(sizeof(intptr_t)  == sizeof(void*), "intptr_t must match pointer size.");
_Static_assert(sizeof(uintptr_t) == sizeof(void*), "uintptr_t must match pointer size.");

/*
 * Boolean
 */
#define bool _Bool
#define true 1
#define false 0

#endif

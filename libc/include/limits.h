#ifndef _LIMITS_H
#define _LIMITS_H

// Chars are spectial :)
#define CHAR_BIT   8
#define CHAR_MIN   0

// Signed min
#define SCHAR_MIN  -127
#define SHRT_MIN   -32768
#define INT_MIN    -2147483648
#define LONG_MIN   -2147483648L
#define LLONG_MIN  -9223372036854775808LL

// Signed max
#define SCHAR_MAX  127
#define SHRT_MAX   32767
#define INT_MAX    2147483647
#define LONG_MAX   2147483647L
#define LLONG_MAX  9223372036854775807LL

// Unsigned max
#define CHAR_MAX   255
#define USHRT_MAX  65535
#define UINT_MAX   4294967295U
#define ULONG_MAX  4294967295UL
#define ULLONG_MAX 18446744073709551615ULL

#endif

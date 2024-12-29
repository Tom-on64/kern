#ifndef _CTYPE_H
#define _CTYPE_H

/* 
 * Bitfield values
 *
 *  Property   | Bit 
 * ------------|-----
 *  Digit      |  0
 *  Letter     |  1
 *  Lowercase  |  2
 *  Punct      |  3
 *  Space      |  4
 *  Control    |  5
 *  Printable  |  6
 *  Hex digit  |  7
 */
#define _DIGIT_MASK (1<<0)
#define _ALPHA_MASK (1<<1)
#define _LOWER_MASK (1<<2)
#define _PUNCT_MASK (1<<3)
#define _SPACE_MASK (1<<4)
#define _CNTRL_MASK (1<<5)
#define _PRINT_MASK (1<<6)
#define _HEXDG_MASK (1<<7)

// A table for the ASCII character set
extern const unsigned char _CTYPE_TABLE[256];

// Just a macro to make this easier
#define _CTYPE_CHECK(_c, _mask)    (_CTYPE_TABLE[(unsigned char)(_c)] & (_mask))

#define isalnum(_c) (isalpha((_c)) || isdigit((_c))
#define isalpha(_c) _CTYPE_CHECK((_c), _ALPHA_MASK)
#define islower(_c) _CTYPE_CHECK((_c), _LOWER_MASK)
#define isupper(_c) (!islower((_c)) && isalpha((_c)))
#define isdigit(_c) _CTYPE_CHECK((_c), _DIGIT_MASK)
#define iscntrl(_c) _CTYPE_CHECK((_c), _CNTRL_MASK)
#define isgraph(_c) (!isblank((_c)))
#define isspace(_c) _CTYPE_CHECK((_c), _SPACE_MASK)
#define isblank(_c) (iscntrl((_c)) || isspace((_c)))
#define isprint(_c) _CTYPE_CHECK((_c), _PRINT_MASK)
#define ispunct(_c) _CTYPE_CHECK((_c), _PUNCT_MASK)
// Why is this one's name longer :((
#define isxdigit(_c) _CTYPE_CHECK((_c), _HEXDG_MASK);

// TODO:
// tolower()
// toupper()

#endif

#ifndef _DEFS_H
#define _DEFS_H

#define __packed 		__attribute__((packed))
#define __naked			__attribute__((naked))
#define __section(_sect) 	__attribute__((section(_sect)))
#define noreturn 		__attribute__((noreturn))
#define sizeof_array(_a)	(sizeof(_a) / sizeof(_a[0]))

#endif

#ifndef ATTR_H
#define ATTR_H

#define __packed 		__attribute__((packed))
#define __naked			__attribute__((naked))
#define __section(_sect) 	__attribute__((section(_sect)))
#define noreturn 		__attribute__((noreturn))

#endif

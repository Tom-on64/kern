#ifndef _ERRNO_H
#define _ERRNO_H

#include <kernel.h>

#define ERRNO_MAX 4095

#define IS_ERR(_ptr) ((uintptr_t)(_ptr) >= (uintptr_t)(-ERRNO_MAX))

#define ERR_PTR(_err) ((void*)(_err))
#define PTR_ERR(_ptr) ((int)(intptr_t)(_ptr))

// TODO: Add correct errno.h numbers
enum {
	SUCCESS = 0,
	ENOMEM,		// Cannot allocate memory
	EPERM,		// Operation not permitted
	ENOENT,		// No entry (no such file or directory)
	EIO,		// Input/Output error
	ENXIO,		// No such device or address
	EAGAIN,		// Resource temporarly busy
	EACCES,		// Permission denied
	EFAULT,		// Bad address
	ENODEV,		// No such device
	EINVAL,		// Invalid argument
	EDOM,		// Numerical argument out of domain
	ERANGE,		// Numerical result out of range
	ENOSYS,		// Function not implemented
	ENOTSUP,	// Not supported
	EILLSEQ,	// Illegal byte sequence
	EOVERFLOW,	// 
	EALIGN,		// Not aligned
};

#endif

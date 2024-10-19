#ifndef _ERRNO_H
#define _ERRNO_H

// Basic POSIX defined error idk
enum {
    EPERM = 1,  // Operation not permitted
    ENOENT,     // No such file or directory
    ESRCH,      // No such process
    EINTR,      // Interrupted system call
    EIO,        // I/O error
    ENXIO,      // No such device or address
    E2BIG,      // Argument list too long
    ENOEXEC,    // Exec format error
    EBADF,      // Bad file descriptor
    ECHILD,     // No child processes
    EAGAIN,     // Resource temporarily unavailable
    ENOMEM,     // Out of memory
    EACCES,     // Permission denied
    EFAULT,     // Bad address
    EBUSY,      // Device or resource busy
    EEXIST,     // File exists
    ENOTDIR,    // Not a directory
    EISDIR,     // Is a directory
    EIMPL,      // TODO: Implementation defined
    ENOMSG,     // No message of desired type
    EINVAL,     // Invalid argument
    ENFILE,     // File table overflow
    EMFILE,     // Too many open files
    ENOTTY,     // Not a typewriter
    ETXTBSY,    // Text file busy
    EFBIG,      // File too large
    ENOSPC,     // No space left on device
    ESPIPE,     // Illegal seek
    EROFS,      // Read-only file system
    EMLINK,     // Too many links
    EPIPE,      // Broken pipe
    EDQUOT,     // Disk quota exceeded
    EDOM,       // Function arg domain error
    ERANGE,     // Out of range
    // TODO: more
    EILSEQ = 42,// Invalid byte seqence
};

extern int errno;

#endif

#ifndef _FCNTL_H
#define _FCNTL_H

/* File access modes */
#define O_CREAT     0x0001  // Create if does not exist
#define O_RDONLY    0x0002  // Read only
#define O_WRONLY    0x0004  // Write only
#define O_RDWR      0x0008  // Read and write
#define O_APPEND    0x0010  // Always writes at the end (appends)
#define O_TRUNC     0x0020  // Truncate file size and position to 0 on Open
#define O_EXCL      0x0040  // Error if file exists (with O_CREAT)
#define O_BIN       0x0080  // Read bytes, not text
#define O_DIRECTORY 0x0100  // Error if file is not a directory
// TODO: more!

/* TODO: File lock */

#endif

#include <stdio.h>
#include <syscall.h>

int getchar(void) { return getc(stdin); }
int putchar(int c) { return putc(c, stdout); }

int getc(FILE* stream) {
    if (!stream) { return EOF; }

    unsigned char ch;
    ssize_t res = read(stream->_file, &ch, 1);
    if (res <= 0) { return EOF; }

    return (int)ch;
}

int putc(int c, FILE* stream) {
    if (!stream) { return EOF; }

    unsigned char ch = (unsigned char)c;
    ssize_t res = write(stream->_file, &c, 1);
    if (res <= 0) { return EOF; }

    return (int)ch;
}

// TODO:
int ungetc(int c, FILE* stream) { return EOF; }


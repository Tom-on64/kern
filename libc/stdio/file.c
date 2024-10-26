#include <stdio.h>
#include <syscall.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

FILE* stdin;
FILE* stdout;
FILE* stderr;

FILE* fopen(const char* path, const char* mode) {
    FILE* fp = (FILE*)malloc(sizeof(FILE));
    if (!fp) { return NULL; }

    int flags = 0;
    if      (strcmp(mode, "r")  == 0) { flags = O_RDONLY; }
    else if (strcmp(mode, "w")  == 0) { flags = O_WRONLY | O_CREAT | O_TRUNC; }
    else if (strcmp(mode, "a")  == 0) { flags = O_WRONLY | O_CREAT | O_APPEND; }
    else if (strcmp(mode, "r+") == 0) { flags = O_RDWR; }
    else if (strcmp(mode, "w+") == 0) { flags = O_RDWR | O_CREAT | O_TRUNC; }
    else if (strcmp(mode, "a+") == 0) { flags = O_RDWR | O_CREAT | O_APPEND; }
    else {  // Unsuppored mode
        free(fp);
        return NULL;
    }

    int fd = open(path, flags);
    if (fd == -1) {
        free(fp);
        return NULL;
    }

    fp->_file = fd;
    fp->_ptr = NULL;
    fp->_size = 0;
    fp->_offset = 0;
    fp->_flag = flags;

    return fp;
}

int fclose(FILE* stream) {
    if (!stream) { return -1; }
    int ret = close(stream->_file);
    free(stream);
    return ret;
}

size_t fread(void* ptr, size_t size, size_t count, FILE* stream) {
    if (!stream || !ptr || size == 0 || count == 0) { return 0; }

    size_t total = size * count;
    ssize_t readb = read(stream->_file, ptr, total);

    if (readb < 0) return 0;

    stream->_offset += readb;
    return readb / size;
}

size_t fwrite(void* ptr, size_t size, size_t count, FILE* stream) {
    if (!stream || !ptr || size == 0 || count == 0) { return 0; }

    size_t total = size * count;
    ssize_t writtenb = write(stream->_file, ptr, total);

    if (writtenb < 0) return 0;

    stream->_offset += writtenb;
    return writtenb / size;
}

// TODO
int fseek(FILE* stream, long offset, int whence);
long ftell(FILE* stream);
void rewind(FILE* stream);
int fflush(FILE* stream);


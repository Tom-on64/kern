#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* Internal definitions */
typedef struct {
    int buflen;
    int offset;
    char* buf;
} printfEnv_t;

/* Helpers */
static void _dynamicPutc(char c, printfEnv_t* env) {
    if (!env->buf) { return; }

    if (env->offset < env->buflen) {
        env->buf[env->offset++] = c;
    } else {
        env->buflen *= 2;
        env->buf = realloc(env->buf, env->buflen);

        // If there's not enough memory, we chatch the NULL pointer in the fwrite() call
        if (!env->buf) { return; }

        env->buf[env->offset++] = c;
    }
}

static void _staticPutc(char c, printfEnv_t* env) {
    if (!env->buf) { return; }

    if (env->offset < env->buflen) {
        env->buf[env->offset] = c;
        env->offset++;
    }
}

/* Impementation */
int vpprintf(void (*outfn)(char c, void* arg), void* arg, const char* fmt, va_list va) { /* TODO */ }

int vsnprintf(char* str, size_t size, const char* fmt, va_list va) {
    printfEnv_t env;
    env.buflen = size;
    env.offset = 0;
    env.buf = str;

    // Format the string 
    int ret = vpprintf(_staticPutc, &env, fmt, va);

    return ret;
}

int vfprintf(FILE* stream, const char* fmt, va_list va) {
    printfEnv_t env;
    env.buflen = 128;
    env.offset = 0;
    env.buf = malloc(env.buflen);
    if (!env.buf) { return -1; }

    // Format the string
    int ret = vpprintf(_dynamicPutc, &env, fmt, va);

    // Write to stream
    if (fwrite(env.buf, 1, ret, stream) != 1) { return -1; }

    free(env.buf);
    return ret;
}

int vprintf(const char* fmt, va_list va) { return vfprintf(stdout, fmt, va); }

// Print using an custom putc function
int pprintf(void (*outfn)(char c), const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int ret = vpprintf(outfn, fmt, va);
    va_end(va);
    return ret;
}

// Print to a buffer
int snprintf(char* str, size_t size, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int ret = vsnprintf(str, size, fmt, va);
    va_end(va);
    return ret;
}

// Print to a stream
int fprintf(FILE* stream, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int ret = vfprintf(stream, fmt, va);
    va_end(va);
    return ret;
}

// Print to stdout
int printf(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int ret = vfprintf(stdout, fmt, va);
    va_end(va);
    return ret;
}


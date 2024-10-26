#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Internal definitions */
typedef struct {
    int buflen;
    int offset;
    char* buf;
} printfEnv_t;

/* Helpers */
// Digit to character
static char _dtoc(int digit) { return digit < 10 ? '0' + digit : 'a' + (digit - 10); }
static double _fabs(double x) { return (x < 0.0) ? -x : x; }
static void _fmtInt(void (*outfn)(char c, void* arg), void* arg, int n, int base, int width, char padc) {
    char buf[32];
    int negative = 0;
    int i = 0;

    if (n < 0 && base == 10) {
        negative = 1;
        n = -n;
    }

    // Convert number, this will be done in reverse
    do {
        buf[i++] = _dtoc(n % base);
        n /= base;
    } while (n > 0);

    // Padding
    while (i < width) { buf[i++] = padc; }
    // Add negative sign
    if (negative) { buf[i++] = '-'; }

    // Funny arrow
    while (i --> 0) { outfn(buf[i], arg); }
}

static void _fmtFloat(void (*outfn)(char c, void* arg), void* arg, double f) {
    int whole = (int)f;
    double frac = _fabs(f - whole);
    int decPlaces = 6;

    // Add negative sign
    if (f < 0) { outfn('-', arg); }

    // Print whole part
    _fmtInt(outfn, arg, whole, 10, 0, ' ');

    // Print decimal point
    outfn('.', arg);

    // Format fractional part
    for (int i = 0; i < decPlaces; i++) {
        frac *= 10;
        int digit = (int)frac;
        outfn('0' + digit, arg);
        frac -= digit;
    }
}

static void _dynamicPutc(char c, void* arg) {
    printfEnv_t* env = (printfEnv_t*)arg;
    if (!env->buf) { return; }

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

static void _staticPutc(char c, void* arg) {
    printfEnv_t* env = (printfEnv_t*)arg;
    if (!env->buf) { return; }

    if (env->offset < env->buflen) {
        env->buf[env->offset] = c;
        env->offset++;
    }
}

/* Impementation */
int vpprintf(void (*outfn)(char c, void* arg), void* arg, const char* fmt, va_list va) {
    int i = 0;

    while (fmt[i] != '\0') {
        if (fmt[i] != '%') { // Just print the character
            outfn(fmt[i++], arg);
            continue;
        }

        i++;    // Consume the %
        if (fmt[i] == '\0') { // End of string
            outfn('%', arg);
            break;
        }

        
        // Parse fmt string
        int width = 0;
        char pad = ' ';

        // Check for zero pad and parse width
        if (isdigit(fmt[i])) {
            if (fmt[i] == '0') { pad = '0'; i++; }

            while (isdigit(fmt[i])) { // Parse width
                width *= 10;
                width += fmt[i++] - '0';
            }
        }

        // Parse specifier
        switch (fmt[i]) {
              case 'c': { // Character
                char c = va_arg(va, char);
                outfn(c, arg);
                break;
            } case 's': { // String
                char* s = va_arg(va, char*);
                while (*s) { outfn(*s++, arg); }
                break;
            } case 'd': { // Decimal number
                int d = va_arg(va, int);
                _fmtInt(outfn, arg, d, 10, width, pad);
                break;
            } case 'x': { // Hex number
                int x = va_arg(va, int);
                _fmtInt(outfn, arg, x, 16, width, pad);
                break;
            } case 'f': { // Float
                double f = va_arg(va, double);
                _fmtFloat(outfn, arg, f);
                break;
            } default: {
                outfn(fmt[i], arg);
                break;
            }
        }

        i++;
    }

    return i;
}

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
int pprintf(void (*outfn)(char c, void* arg), void* arg, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int ret = vpprintf(outfn, arg, fmt, va);
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


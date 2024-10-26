#include <error/debug.h>
#include <ports/serial.h>
#include <stdio.h>
#include <stdarg.h>

static void _dbgPutc(char c, void* arg) {
    (void)arg;
    serialWrite(PORT_COM1, c);
    if (c == '\n') { serialWrite(PORT_COM1, '\r'); }
}

int debugf(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int res = vpprintf(_dbgPutc, NULL, fmt, va);
    va_end(va);
    return res;
}


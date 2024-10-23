#include <error/debug.h>
#include <ports/serial.h>
#include <stdarg.h>

int debugf(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    serialPrint(PORT_COM1, fmt);    // TODO: use a printf
    int res = 0;
    va_end(va);
    return res;
}


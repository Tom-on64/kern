#include <kernel.h>
#include <serial.h>
#include <stdarg.h>
#include <ctype.h>

uint8_t console_loglevel = CONSOLE_LOGLEVEL;

int printk(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int ret = vprintk(fmt, ap);
	va_end(ap);
	return ret;
}

int vprintk(const char* fmt, va_list ap) {
	int loglevel = LOGLEVEL_DEFAULT;
	if (*fmt == KERN_SOH_ASCII) {
		fmt++;
		if (isdigit(*fmt)) loglevel = *fmt++ - '0';
	}

	if (loglevel > console_loglevel) return 0;

	// TODO: debugf("[%4d.%6d] ", seconds, milliseconds);
	return vdebugf(fmt, ap);
}


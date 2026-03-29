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

	// Loglevel colors
	switch (loglevel) {
	case LOGLEVEL_EMERG:   debugf("\x1b[1;31m"); break; // bold red
	case LOGLEVEL_ALERT:   debugf("\x1b[1;31m"); break; // bold red
	case LOGLEVEL_CRIT:    debugf("\x1b[31m");   break; // red
	case LOGLEVEL_ERROR:   debugf("\x1b[31m");   break; // red
	case LOGLEVEL_WARNING: debugf("\x1b[33m");   break; // yellow
	case LOGLEVEL_NOTICE:  debugf("\x1b[34m");   break; // green
	case LOGLEVEL_INFO:    debugf("\x1b[37m");   break; // white
	case LOGLEVEL_DEBUG:   debugf("\x1b[2;32m"); break; // green
	}

	// TODO: debugf("[%4d.%6d] ", seconds, milliseconds);
	int ret = vdebugf(fmt, ap);
	debugf("\x1b[0m");
	return ret;
}


#include <kernel.h>
#include <string.h>

#include <vsnprintf.h>

int pf_isdigit(char c) {
	if (c >= '0' && c <= '9') return 1;
	return 0;
}

char* pf_itoa(uint32_t num, uint8_t base) {
	static char buf[32];
	
	if (base == 0 || base > 16) return NULL;
	
	if (num == 0) {
		buf[0] = '0';
		buf[1] = '\0';
		return &buf[0];
	}

	int i;
	for (i = 30; num && i; i--, num /= base) {
		buf[i] = "0123456789abcdef"[num % base];	
	}
	
	return &buf[i + 1];
}

int vsnprintf(char* buf, size_t len, char* fmt, va_list args) {
	size_t i = 0;

	while (*fmt != '\0' && i < len) {
		if (*fmt != '%') {
			if (i < len) buf[i++] = *fmt++;
			continue;
		}

		fmt++; // Consume the '%'
		if (*fmt == '\0') {
			if (i < len) buf[i++] = '%';
			break;
		}

		// Parse the format specifier
		int width = 0;
		char pad = ' ';

		if (pf_isdigit(*fmt)) {
			if (*fmt == '0') { pad = '0'; fmt++; }

			while (pf_isdigit(*fmt)) {
				width *= 10;
				width += *fmt++ - '0';
			}
		}

		char* s;
		switch (*fmt) {
		case 'c': // Char
			if (i < len) buf[i++] = (char)va_arg(args, int);
			break;
		case 'd': // Int (TODO: %d for signed, %u for unsigned
			s = pf_itoa(va_arg(args, unsigned int), 10);
			goto string;
		case 'x': // Unsigned hex int
			s = pf_itoa(va_arg(args, unsigned int), 16);
			goto string;
		// TODO: %f for float
		case 's': // String
			s = va_arg(args, char*);
		string: // Prints the string in s
			if (s == NULL) s = "(null)";
			width -= strlen(s);
			while (width-- >= 0 && i < len) buf[i++] = pad;
			while (*s != '\0' && i < len) buf[i++] = *s++;
			break;
		default: // Something else
			if (i < len) buf[i++] = '%';
			if (i < len) buf[i++] = *fmt;
			break;
		}

		fmt++;
	}

	if (i < len) buf[i++] = '\0';

	return i;
}


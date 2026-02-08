#include <kernel.h>
#include <string.h>

#include <vsnprintf.h>

static inline int _isdigit(int c) { return (c >= '0' && c <= '9') ? 1 : 0; }

static size_t _utoa(char* out, uint64_t v, uint32_t base) {
	static char* digits = "0123456789abcdef";

	size_t i = 0;
	if (v == 0) {
		out[i++] = '0';
		return i;
	}

	while (v) {
		out[i++] = digits[v % base];
                v /= base;
	}
	return i;
}

static void _putc(char* buf, size_t n, size_t* idx, char c) {
	if (*idx < n - 1) buf[*idx] = c;
	(*idx)++;
}

static void _puts(char* buf, size_t n, size_t* idx, char* s, size_t len) {
	for (size_t i = 0; i < len; i++) _putc(buf, n, idx, s[i]);
}

static void fmt_string(char* buf, size_t n, size_t* idx, char* s, size_t len, int width, char pad, int left) {
	int pad_len = width > (int)len ? width - (int)len : 0;
	if (!left) { while (pad_len--) _putc(buf, n, idx, pad); }
	_puts(buf, n, idx, s, len);
	if (left) { while (pad_len--) _putc(buf, n, idx, pad); }
}

static void fmt_int(char* buf, size_t n, size_t* idx, int64_t v, int width, char pad, int left) {
	char tmp[32];
	size_t len;
	int neg = v < 0;

	uint64_t uv = neg ? -v : v;
	len = _utoa(tmp, uv, 10);
	
	if (neg) tmp[len++] = '-';

	for (size_t i = 0; i < len / 2; i++) {
		char c = tmp[i];
		tmp[i] = tmp[len - 1 - i];
		tmp[len - 1 - i] = c;
	}

	fmt_string(buf, n, idx, tmp, len, width, pad, left);
}

static void fmt_uint(char* buf, size_t n, size_t* idx, uint64_t v, uint32_t base, int width, char pad, int left) {
	char tmp[32];
	size_t len = _utoa(tmp, v, base);

	for (size_t i = 0; i < len / 2; i++) {
		char c = tmp[i];
		tmp[i] = tmp[len - 1 - i];
		tmp[len - 1 - i] = c;
	}

	fmt_string(buf, n, idx, tmp, len, width, pad, left);
}

// WARN: I took this from somewhere and idk how good it is and i'm too lazy to check
static void fmt_float(char* buf, size_t n, size_t* idx, double v, int prec, int width, char pad, int left) {
	if (prec < 0) prec = 6;

	if (v < 0) {
		_putc(buf, n, idx, '-');
		v = -v;
		if (width > 0) width--;
	}

	long long ip = (long long)v;
	double frac = v - (double)ip;

	char tmp[64];
	size_t len = _utoa(tmp, ip, 10);

	for (size_t i = 0; i < len / 2; i++) {
		char c = tmp[i];
		tmp[i] = tmp[len - 1 - i];
		tmp[len - 1 - i] = c;
	}

	size_t total = len + 1 + prec;
	int pad_len = width > (int)total ? width - (int)total : 0;

	if (!left) { while (pad_len--) _putc(buf, n, idx, pad); }

	_puts(buf, n, idx, tmp, len);
	_putc(buf, n, idx, '.');

	for (int i = 0; i < prec; i++) {
		frac *= 10.0;
		int d = (int)frac;
		_putc(buf, n, idx, '0' + d);
		frac -= d;
	}

	if (left) { while (pad_len--) _putc(buf, n, idx, pad); }
}

int vsnprintf(char* buf, size_t n, char* fmt, va_list ap) {
	size_t idx = 0;

	for (; *fmt != '\0'; fmt++) {
		if (*fmt != '%') {
			_putc(buf, n, &idx, *fmt);
			continue;
		}

		fmt++;

		int left = 0;
		char pad = ' ';
		int width = 0;
		int prec = -1;

		if (*fmt == '-') {
			left = 1;
			fmt++;
		}

		if (*fmt == '0') {
			pad = '0';
			fmt++;
		}

		while (_isdigit((int)*fmt)) {
			width = width * 10 + (*fmt - '0');
			fmt++;
		}
		
		if (*fmt == '.') {
			fmt++;
			prec = 0;
			while (_isdigit((int)*fmt)) {
				prec = prec * 10 + (*fmt - '0');
				fmt++;
			}
		}

		switch (*fmt) {
		case 'd': fmt_int(buf, n, &idx, va_arg(ap, int), width, pad, left); break;
		case 'u': fmt_uint(buf, n, &idx, va_arg(ap, unsigned int), 10, width, pad, left); break;
		case 'x': fmt_uint(buf, n, &idx, va_arg(ap, unsigned int), 16, width, pad, left); break;
		case 'f': fmt_float(buf, n, &idx, va_arg(ap, double), prec, width, pad, left); break;
		case 's': {
			char* s = va_arg(ap, char*);
			if (s == NULL) s = "(null)";
			size_t len = strlen(s);
			if (prec >= 0) len = min(len, (size_t)prec);
			fmt_string(buf, n, &idx, s, len, width, pad, left);
			} break;
		case 'c': _putc(buf, n, &idx, (char)va_arg(ap, int)); break;
		case '%': _putc(buf, n, &idx, '%'); break;
		default:
			// Undo all the parsing we just did :')
			_putc(buf, n, &idx, '%');
			if (left) _putc(buf, n, &idx, '-');
			if (pad == '0') _putc(buf, n, &idx, '0');
			if (width > 0) fmt_uint(buf, n, &idx, width, 10, 0, 0, 0);
			if (prec != -1) {
				_putc(buf, n, &idx, '.');
				fmt_uint(buf, n, &idx, prec, 10, 0, 0, 0);
			}
			_putc(buf, n, &idx, *fmt);
			break;
		}
	}

	if (n) buf[min(idx, n - 1)] = '\0';
	return (int)idx;
}

int snprintf(char *buf, size_t n, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int ret = vsnprintf(buf, n, fmt, ap);
	va_end(ap);
	return ret;
}


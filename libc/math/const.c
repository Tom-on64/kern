#include <math.h>

double nan(const char* tagp) {
    (void)tagp;

    // Create a NaN bit pattern in IEEE 754
    uint64_t nanBits = 0x7ff8000000000000ULL;
    double result;
    memcpy(&result, &nanBits, sizeof(result));
    return result;
}

double inf() {
    // Create an infinity bit pattern in IEEE 754
    uint64_t infBits = 0x7ff0000000000000ULL;
    double result;
    memcpy(&result, &infBits, sizeof(result));
    return result;
}

int isfinite(double x) {
    uint64_t bits;
    memcpy(&bits, &x, sizeof(bits));
    uint64_t exponent = (bits >> 52) & 0x7ff;
    return exponent != 0x7ff;
}

int isinf(double x) {
    uint64_t bits;
    memcpy(&bits, &x, sizeof(bits));
    uint64_t exponent = (bits >> 52) & 0x7ff;
    uint64_t mantissa = bits & 0xfffffffffffff;
    return (exponent == 0x7ff && mantissa == 0);
}

int isnan(double x) {
    uint64_t bits;
    memcpy(&bits, &x, sizeof(bits));
    uint64_t exponent = (bits >> 52) & 0x7ff;
    uint64_t mantissa = bits & 0xfffffffffffff;
    return (exponent == 0x7ff && mantissa != 0);
}


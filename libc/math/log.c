#include <math.h>

#define _LOG_TERMS  20

double log(double x) {
    if (x <= 0.0) return -INFINITY;

    double result = 0.0;
    double term = (x - 1.0) / (x + 1.0);
    double termSquared = term*term;
    double numerator = term;

    for (int i = 1; i < _LOG_TERMS; i++) {
        result += numerator / (double)i;
        numerator *= termSquared;
    }

    return 2.0 * result;
}

double log2(double x) {
    if (x <= 0.0) return -INFINITY;
    return log(x) / M_LN2;
}

double log10(double x) {
    if (x <= 0.0) return -INFINITY;
    return log(x) / M_LN10;
}

double logn(double x, double n) {
    if (x <= 0.0) return -INFINITY;
    return log(x) / log(n);
}


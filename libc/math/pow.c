#include <math.h>

#define _POW_LOOP_MAX   60

double sqrt(double x) {
    if (x < 0.0) return -NAN;

    double result = x;
    double oldResult = 0.0;

    while (result != oldResult) {
        oldResult = result;
        result = 0.5 * (result + x/result);
    }

    return result;
}

double cbrt(double x) {
    double result = x;
    double oldResult = 0.0;

    while (result != oldResult) {
        oldResult = result;
        result = (2.0 * result + x/(result * result)) / 3.0;
    }

    return result;
}

double hypot(double x, double y) {
    return sqrt(x*x + y*y);
}

double pow(double base, double expn) { // exp is a function so we have to use expn
    if (base == 0.0 && expn == 0.0) return NAN;
    if (base == 0.0) return 0.0;
    if (expn == 0.0) return 1.0;

    return exp(expn * log(base));
}


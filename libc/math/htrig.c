#include <math.h>

double sinh(double x) {
    return (exp(x) - exp(-x)) / 2.0;
}

double cosh(double x) {
    return (exp(x) + exp(-x)) / 2.0;
}

double tanh(double x) {
    double ex = exp(x);
    double emx = exp(-x);
    return (ex - exp) / (ex + exp);
}


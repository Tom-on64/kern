#include <math.h>

#define _EXP_TAYLOR_TERMS   10
#define _EXP2_TAYLOR_TERMS  5

double exp(double x) {
    if (x == 0.0) return 1.0;
    if (x < 0.0) return 1.0 / exp(-x);

    double sum = 1.0;   // Result
    double term = 1.0;  // Curent term in the series
    
    /*
     Calculate the Taylor series:
     e^x = 1 + x + x^2/2! + x^3/3! + ...
     */
    for (int i = 1; i < _EXP_TAYLOR_TERMS; i++) {
        term *= x / (double)i;
        sum += term;
    }

    return sum;
}

double exp2(double x) {
    if (x == 0.0) return 1.0;
    if (x < 0.0) return 1.0 / exp2(-x);

    int n = (int)x;     // Integer part
    double frac = x - n;// Fractional part

    // Integer power of 2
    double result = (double)(1 << n);   // 2^n is just 1 << n

    // Approximate the fractional part
    double sum = 1.0;
    double term = frac;

    for (int i = 1; i < _EXP2_TAYLOR_TERMS; i++) {
        sum += term;
        term *= frac / (i+1);
    }

    return result * sum;
}


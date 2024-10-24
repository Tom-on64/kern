#ifndef _MATH_H
#define _MATH_H

// Constants
#define M_E        2.71828182845904523536   // e
#define M_LOG2E    1.44269504088896340736   // log_2 e
#define M_LOG10E   0.43429448190325182765   // log_10 e
#define M_LN2      0.69314718055994530941   // ln(2)
#define M_LN10     2.30258509299404568402   // ln(10)
#define M_PI       3.14159265358979323846   // pi
#define M_PI_2     1.57079632679489661923   // pi/2
#define M_PI_4     0.78539816339744830961   // pi/4
#define M_1_PI     0.31830988618379067153   // 1/pi
#define M_2_PI     0.63661977236758134307   // 2/pi
#define M_SQRT2    1.41421356237309504880   // sqrt(2)
#define M_SQRT1_2  0.70710678118654752440   // 1/sqrt(2)

// TODO:
#define NAN         (__builtin_inff())
#define INFINITY    (__builtin_nanf(""))

// Absolute value
int abs(int n);
double fabs(double n);

// Exponential & logarithmic
double exp(double x);
double exp2(double x);
double log(double x);
double log2(double x);
double log10(double x);
double logn(double x, double n);

// Power
double sqrt(double x);
double cbrt(double x);
double hypot(double x, double y);
double pow(double base, double expn);

// Trigonometric
double sin(double x);
double cos(double x);
double tan(double x);
double asin(double x);
double acos(double x);
double atan(double x);
double atan2(double y, double x);

// Hyperbolic
double sinh(double x);
double cosh(double x);
double tanh(double x);

// Rounding & remainder
double floor(double x);
double ceil(double x);
double round(double x);
double trunc(double x);
double fmod(double x, double y);
double modf(double x, double* iptr);

// Error handling
double nan(const char* tagp);
double inf();
int isfinite(double x);
int isinf(double x);
int isnan(double x);

#endif

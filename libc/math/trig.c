#include <math.h>

double sin(double x) {
    double res;
    __asm__ volatile (
        "fldl %1\n"     // Load x to the FPU stack
        "fsin\n"        // Compute sin(x)
        "fstpl %0"      // Store result
        : "=m"(res) : "m"(x)
    );
    return res;
}

double cos(double x) {
    double res;
    __asm__ volatile (
        "fldl %1\n"     // Load x to the FPU stack
        "fcos\n"        // Compute cos(x)
        "fstpl %0"      // Store result
        : "=m"(res) : "m"(x)
    );
    return res;
}


double tan(double x) {
    return sin(x) / cos(x);
}

double asin(double x) {
    return atan2(x, sqrt(1 - x*x));
}

double acos(double x) {
    return M_PI_2 - asin(x);
}

double atan(double x) {
    double res;
    __asm__ volatile (
        "fld1\n"        // Load 1.0 to the FPU stack
        "fldl %1\n"     // Load x to the FPU stack
        "fpatan\n"      // Compute atan(x)
        "fstpl %0"      // Store result
        : "=m"(res) : "m"(x)
    );
    return res;
}

double atan2(double y, double x) {
    double res;
    __asm__ volatile (
        "fldl %2\n"       // Load x to the FPU stack
        "fldl %1\n"       // Load y to the FPU stack
        "fpatan\n"        // Compute atan2(y, x)
        "fstpl %0"        // Store the result
        : "=m"(res) : "m"(y), "m"(x)
    );
    return res;
}


#include <math.h>

double round(double x) {
    double res;
    __asm__ volatile (
        "fldl %1\n"       // Load x to the FPU stack
        "frndint\n"       // Round to nearest integer
        "fstpl %0"        // Store result
        : "=m"(res)
        : "m"(x)
    );
    if ((x - res) == 0.5 || (x - res) == -0.5) {
        // Halfway cases: round away from zero
        res += (x > 0) ? 1.0 : -1.0;
    }
    return res;
}

double floor(double x) {
    double res = round(x);
    if (res > x) { res -= 1.0; }
    return res;
}

double ceil(double x){
    double res = round(x);
    if (res < x) { res += 1.0; }
    return res;
}

double trunc(double x) {
    double res = round(x);
    if (x < 0 && x != res) { res -= 1.0; }
    return res;
}

double fmod(double x, double y) {
    return x - y * floor(x / y);
}

double modf(double x, double* iptr) {
    double whole = trunc(x);
    *iptr = whole;
    return x - whole;
}


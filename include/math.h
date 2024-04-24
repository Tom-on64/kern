#ifndef MATH_H
#define MATH_H

// Constants
#define PI 3.141592653589 // Should be good enough
#define E  2.718281828459

// Macros
#define abs(x) ( (x) > 0 ? (x) : -(x) )
#define sign(x) ( (x) == 0 ? 0 : ((x) < 0 ? -1 : 1 ) )

#define floor(x) ( (int)(x) ) // C floors floats when converting to ints
#define round(x) ( (int)((x) + 0.5) ) // Normal rounding
#define ceil(x)  ( (int)((x) + 1) ) // Always rounds up

#endif

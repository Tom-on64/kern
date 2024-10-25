#ifndef _FLOAT_H
#define _FLOAT_H

#define FLT_DIG        6          // Float precision: 6 decimal digits
#define DBL_DIG        15         // Double precision: 15 decimal digits
#define LDBL_DIG       18         // Long double precision: 18 decimal digits

#define FLT_MAX        3.40282347e+38F   // Maximum float value
#define DBL_MAX        1.7976931348623157e+308  // Maximum double value
#define LDBL_MAX       1.189731495357231765e+4932L  // Max long double

#define FLT_MIN        1.17549435e-38F   // Minimum positive normalized float
#define DBL_MIN        2.2250738585072014e-308  // Min positive double
#define LDBL_MIN       3.36210314311209350626e-4932L  // Min positive long double

#define FLT_EPSILON    1.19209290e-7F    // Smallest float increment
#define DBL_EPSILON    2.2204460492503131e-16  // Smallest double increment
#define LDBL_EPSILON   1.08420217248550443401e-19L  // Smallest long double increment

#define FLT_MAX_EXP    128       // Max exponent for float
#define DBL_MAX_EXP    1024      // Max exponent for double
#define LDBL_MAX_EXP   16384     // Max exponent for long double

#define FLT_MIN_EXP    -125      // Min exponent for float
#define DBL_MIN_EXP    -1021     // Min exponent for double
#define LDBL_MIN_EXP   -16381    // Min exponent for long double

#define FLT_RADIX      2         // Floating-point base (usually 2)
#define FLT_MANT_DIG   24        // Number of digits in float mantissa
#define DBL_MANT_DIG   53        // Number of digits in double mantissa
#define LDBL_MANT_DIG  64        // Number of digits in long double mantissa

#endif

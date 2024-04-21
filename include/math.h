#ifndef MATH_H
#define MATH_H

#define abs(x) ( (x) > 0 ? (x) : -(x) )
#define sign(x) ( (x) == 0 ? 0 : ((x) < 0 ? -1 : 1 ) )
#define round(x) ( (int)((x) + 0.5) )

#endif

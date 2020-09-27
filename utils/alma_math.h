#ifndef _ALMA_MATH_H_
#define _ALMA_MATH_H_

#include <mbed.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) >= (b) ? (a) : (b))

uint8_t fast_atoi(const char *p);

#endif // _ALMA_MATH_H_

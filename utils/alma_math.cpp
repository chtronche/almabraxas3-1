#include "alma_math.h"

static int fast_x10[] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 };

uint8_t fast_atoi(const char *p) {
  return fast_x10[p[0] - '0'] + p[1] - '0';
}

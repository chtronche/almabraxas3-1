// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#include "LinearMapper.h"

void LinearMapper::computeFactor(int from0, float to0, int from1, float to1) {
  a = (to1 - to0) / (from1 - from0);
  b = to0 - a * from0;
}

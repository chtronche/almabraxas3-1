// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#include "LinearMapper.h"

void LinearMapper::computeFactor(int from0, float to0, int from1, float to1) {
  a = (to1 - to0) / (from1 - from0);
  b = to0 - a * from0;
}

LinearMapper::LinearMapper(int from0, float to0, int from1, float to1) {
  from1 = from0 + 1; // To avoid division by zero
  resetPoint(from0, to0, false);
  resetPoint(from1, to1, true);
}

void LinearMapper::resetPoint(int from, float to, bool lowPoint) {
  if (lowPoint) {
    f0 = from;
    t0 = to;
  } else {
    f1 = from;
    t1 = to;
  }
  computeFactor(f0, t0, f1, t1);
}


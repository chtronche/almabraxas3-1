// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#include "LinearMapper.h"
#include "NVStore.h"
#include "vars.h"

void LinearMapper::computeFactor(int16_t from0, float to0, int16_t from1, float to1) {
  a = (to1 - to0) / (from1 - from0);
  b = to0 - a * from0;
}

LinearMapper::LinearMapper(int from0, float to0, int from1, float to1) {
  from1 = from0 + 1; // To avoid division by zero
  resetPoint(from0, to0, false);
  resetPoint(from1, to1, true);
}

void LinearMapper::resetPoint(int from, float to, bool highPoint) {
  if (highPoint) {
    f0 = from;
    t0 = to;
  } else {
    f1 = from;
    t1 = to;
  }
  computeFactor(f0, t0, f1, t1);
}

NVLinearMapper::NVLinearMapper(const char *f0Name, const char *t0Name, const char *f1Name,
			       const char *t1Name)
  :_f0Name(f0Name), _t0Name(t0Name), _f1Name(f1Name), _t1Name(t1Name) { }

void NVLinearMapper::retrieve() {
  NV<int16_t>::get(_f0Name, &f0);
  NV<float>::get(_t0Name, &t0);

  vars_register(_f0Name, &f0);
  vars_register(_t0Name, &t0);

  NV<int16_t>::get(_f1Name, &f1);
  NV<float>::get(_t1Name, &t1);

  vars_register(_f1Name, &f1);
  vars_register(_t1Name, &t1);

  const_cast<NVLinearMapper &>(*this).computeFactor(f0, t0, f1, t1);
}

void NVLinearMapper::resetPoint(int16_t from, float to, bool highPoint) {
  if (highPoint) {
    NV<int16_t>::set(_f1Name, &f1, from);
    NV<float>::set(_t1Name, &t1, to);
  } else {
    NV<int16_t>::set(_f0Name, &f0, from);
    NV<float>::set(_t0Name, &t0, to);
  }
  const_cast<NVLinearMapper &>(*this).computeFactor(f0, t0, f1, f1);
}

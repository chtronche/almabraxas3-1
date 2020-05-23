#include <stdint.h>

#include "NVStore.h"
#include "NVStore_osd.h"

template<class T> void NV<T>::get(const char *key, T *var) {
  nvStore.get(key, var, sizeof(T));
}

template<class T> T NV<T>::get(const char *key) {
  T res;
  nvStore.get(key, &res, sizeof(T));
  return res;
}

template<class T> void NV<T>::set(const char *key, T *var, T value) {
  if (*var == value) return;
  *var = value;
  nvStore.set(key, var, sizeof(T), 0);
}

template struct NV<int16_t>; // i
template struct NV<uint16_t>;// u
template struct NV<uint32_t>;// U
template struct NV<float>; // f
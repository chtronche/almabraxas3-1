#include <stdint.h>

#include "NVStore.h"
#include "NVStore_osd.h"
#include "sdlog.h"

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
  char buffer[128];
  snprintf(buffer, 128, "set %s to ", key);
  char *p = strchr(buffer, '\0');
  int left = p - buffer;
  NV_snprintf(p, left, key, var);
  sdlog("nv", buffer);
}

template<class T> void NV<T>::setFirstValue(const char *key, T value) {
  char buffer[sizeof(T)];
  int error = nvStore.get(key, buffer, sizeof(T));
  if (!error || error != MBED_ERROR_ITEM_NOT_FOUND) return;
  nvStore.set(key, &value, sizeof(T), 0);
}

void NV_snprintf(char *buffer, size_t size, const char *key, void *var) {
  uint32_t U;
  switch(key[0]) {
  case 'i':
    snprintf(buffer, size, "%d", *static_cast<int16_t *>(var));
    break;

  case 'u':
    snprintf(buffer, size, "%d", *static_cast<uint16_t *>(var));
    break;

  case 'U':
    U = *static_cast<uint32_t *>(var);
    snprintf(buffer, size, "%ld %lxd", U, U);
    break;

  case 'f':
    snprintf(buffer, size, "%f", *static_cast<float *>(var));
    break;

  default:
    break;
  }
}

template struct NV<int16_t>; // i
template struct NV<uint16_t>;// u
template struct NV<uint32_t>;// U
template struct NV<float>; // f

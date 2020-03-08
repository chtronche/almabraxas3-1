#include "NVStore.h"
#include "NVStore_osd.h"

void getu(const char *key, uint16_t *var) {
  nvStore.get(key, var, sizeof(uint16_t));
}

void setu(const char *key, uint16_t *var, uint16_t value) {
  if (*var == value) return;
  *var = value;
  nvStore.set(key, var, sizeof(uint16_t), 0);
}

void getU(const char *key, uint32_t *var) {
  nvStore.get(key, var, sizeof(uint32_t));
}

void setU(const char *key, uint32_t *var, uint32_t value) {
  if (*var == value) return;
  *var = value;
  nvStore.set(key, var, sizeof(uint32_t), 0);
}

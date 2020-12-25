#ifndef _ALMA_NVSTORE_H_
#define _ALMA_NVSTORE_H_

#include <stddef.h>

template<class T> struct NV {
  static void get(const char *key, T *var);
  static T get(const char *key);

// There is no point in returning an error code, since the robot is
// in the field and can't react. If we can't change some
// configuration remotely, we can't that's it

  static void set(const char *key, T *var, T value);
  static void setFirstValue(const char *key, T value);

};

void NVStore_init();
void NVStore_dump(); // Call when sdlog is up
void NV_snprintf(char *buffer, size_t size, const char *key, void *var);

#endif // _ALMA_NVSTORE_H_

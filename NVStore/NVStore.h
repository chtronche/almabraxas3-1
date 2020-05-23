#ifndef _ALMA_NVSTORE_H_
#define _ALMA_NVSTORE_H_

// There is no point in returning an error code, since the robot is
// in the field and can't react. If we can't change some
// configuration remotely, we can't that's it

// void set(const char *key, const void *buffer, size_t size);
// int get(const char *key, void *buffer, size_t buffer_size, size_t *actual_size=NULL);

template<class T> struct NV {
  static void get(const char *key, T *var);
  static T get(const char *key);
  static void set(const char *key, T *var, T value);
};

void NVStore_init();

#endif // _ALMA_NVSTORE_H_

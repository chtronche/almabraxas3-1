#ifndef _ALMA_NVSTORE_H_
#define _ALMA_NVSTORE_H_

#include "mbed.h"

// There is no point in returning an error code, since the robot is
// in the field and can't react. If we can't change some
// configuration remotely, we can't that's it

// void set(const char *key, const void *buffer, size_t size);

// int get(const char *key, void *buffer, size_t buffer_size, size_t *actual_size=NULL);

// void setU(const char *key, uint32_t &var, uint32_t v);
// void setS(const char *key, char *&var, const char *v);

void getu(const char *key, uint16_t *var);
void setu(const char *key, uint16_t *var, uint16_t value);

void getU(const char *key, uint32_t *var);
void setU(const char *key, uint32_t *var, uint32_t value);

void NVStore_init();

#endif // _ALMA_NVSTORE_H_

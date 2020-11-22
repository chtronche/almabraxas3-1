#include <mbed.h>

#include "NVStore.h"
#include "reporting.h"
#include "sdlog.h"
#include "vars.h"

static uint32_t _flags;

static const int FLAG_MAX = 1;

bool flag_copy_radio_to_serial = false;
bool flag_copy_sdlog_to_serial = false;

static bool *_flagArray[] = {
  &flag_copy_radio_to_serial,
  &flag_copy_sdlog_to_serial,
};

void setFlag(uint8_t flag, bool value) {
  if (flag > FLAG_MAX) return;
  *_flagArray[flag] = value;
  uint32_t mask = 1 << flag;
  NV<uint32_t>::set("UFlags", &_flags, (_flags & ~mask) | (mask & uint32_t(value)));
  char buffer[128];
  snprintf(buffer, 128, "flags = %lx\n", _flags);
  sdlog("report", buffer);
}

// Needs NVStore_init

void flag_init() {
  NV<uint32_t>::get("UFlags", &_flags);
  vars_register("UFlags", &_flags);
  for(int i = 0; i <= FLAG_MAX; i++) {
    *_flagArray[i] = _flags & (1 << i);
  }
  flag_copy_sdlog_to_serial = true;
}

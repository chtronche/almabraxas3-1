#include <mbed.h>

#include "system.h"

static mbed_error_ctx *past = NULL;

void mbed_error_reboot_callback(mbed_error_ctx *error_context) {
  past = error_context;
}

void autoreboot_init() {
  if (!past) return;
  sdlog("autoreboot", 
}

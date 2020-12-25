#include <mbed.h>

#include "NVStore.h"
#include "sdlog.h"

char __CRASH_DATA_RAM_START__[256]
  __attribute__ ((aligned(8)))
  __attribute__ ((section(".noinit")));

static mbed_error_ctx *past = NULL;

void mbed_error_reboot_callback(mbed_error_ctx *error_context) {
  past = error_context;
}

void autoreboot_init() {
  if (!past) return;
  NV<uint32_t>::setFirstValue("UAutoRbt", 0);
  uint32_t UAutoRbt = NV<uint32_t>::get("UAutoRbt");
  NV<uint32_t>::set("UAutoRbt", &UAutoRbt, UAutoRbt + 1);

  char buffer[128];
  snprintf(buffer, 128, "Error Status 0x%x Code: %d Module: %d",
	   past->error_status, MBED_GET_ERROR_CODE(past->error_status),
	   MBED_GET_ERROR_MODULE(past->error_status));
  sdlog("autoreboot", buffer);

  snprintf(buffer, 128, "Location: 0x%lx\tError Value: 0x%lx\tThread Id: 0x%lx",
	   past->error_address, past->error_value, past->thread_id);
  sdlog("autoreboot", buffer);
  
}

#include <stdio.h>

#include "adebug.h"
#include "alma_clock.h"
#include "alma_flags.h"
#include "mbed.h"
#include "nav.h"
#include "NVStore.h"
#include "ping.h"
#include "powerManager.h"
#include "reporting.h"
#include "sd.h"
#include "sdlog.h"
#include "system.h"

Ping ping;

int main()
{
  alma_clock_init();
  chenillard();
  reporting_serial_init();
  NVStore_init();
  sd_init();
  autoreboot_init();
  flag_init();
  NVStore_dump();
  satellite_init();
  sdlog("main", "starting");
  reporting_init();
  powerManager_init();
  compass_init();
  nav_init();
  gpsLoop_init();
  while(1) {
    alma_clock_advance();
    reporting_loop();
    wait_ms(100);
  }
}

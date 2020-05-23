#include <stdio.h>

#include "adebug.h"
#include "mbed.h"
#include "nav.h"
#include "NVStore.h"
#include "ping.h"
#include "powerManager.h"
#include "reporting.h"

Ping ping;

int main()
{
  chenillard();
  reporting_serial_init();
  NVStore_init();
  reporting_init();
  powerManager_init();
  nav_init();
  gpsLoop_init();
  while(1) {
    reporting_loop();
    wait_ms(100);
  }
}

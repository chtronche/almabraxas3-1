#include <stdio.h>

#include "adebug.h"
#include "mbed.h"
#include "ping.h"
#include "reporting.h"

Ping ping;

int main()
{
  chenillard();
  reporting_init();
  while(1) {
    reporting_loop();
    wait_ms(100);
  }
}

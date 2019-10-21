#include <stdio.h>

#include "adebug.h"
#include "mbed.h"
#include "ping.h"
#include "reporting.h"

DigitalOut myled(LED1);

FlashIAP flash;
Ping ping;

int main()
{
  chenillard();
  reporting_init();
  flash.init();
  printf("%lx %ld %ld\n", flash.get_flash_start(), flash.get_flash_size(), flash.get_page_size());
  while(1) {
    reporting_loop();
    wait_ms(1000);
    myled = !myled;
  }
}

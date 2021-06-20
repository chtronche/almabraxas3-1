#include "adebug.h"
#include "mbed.h" // for wait_ms

void chenillard() {
  led_set(0, 0x1f);

  led_set(led_white, 0);
  wait_ms(200);
  led_set(led_blue, 0);
  wait_ms(200);
  led_set(led_yellow, 0);
  wait_ms(200);
  led_set(led_green, 0);
  wait_ms(200);
  led_set(led_red, 0);
  wait_ms(200);
  
  unsigned v = 1;
  // for(int n = 5; n; n--) {
  //   led_set(v, 0);
  //   v = v << 1;
  //   wait_ms(200);
  // }
  led_set(0, 0x1f);
}

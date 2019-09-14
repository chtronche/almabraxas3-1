#include "adebug.h"
#include "mbed.h" // for wait_ms

void chenillard() {
  unsigned v = 1;
  for(int n = 5; n; n--) {
    led(v, 0x1f);
    v = v << 1;
    wait_ms(200);
  }
  led(0, 0x1f);
}

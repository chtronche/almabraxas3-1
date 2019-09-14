#include "debug.h"
#include "mbed.h"
#include "wiring.h"

static DigitalOut redLed(DEBUG_LED_RED);
static DigitalOut greenLed(DEBUG_LED_GREEN);
static DigitalOut yellowLed(DEBUG_LED_YELLOW);
static DigitalOut blueLed(DEBUG_LED_BLUE);
static DigitalOut whiteLed(DEBUG_LED_WHITE);

static DigitalOut *_driver[] = {
    &redLed, &greenLed, &yellowLed, &blueLed, &whiteLed, NULL
    };
    
void led(int n, unsigned mask) {
  unsigned bit = 1;
  for(DigitalOut **p = _driver; *p; p++) {
    if (mask & bit) (*p)->write(n&bit);
    bit = bit << 1;
  }
}

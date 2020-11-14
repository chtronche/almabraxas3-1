#include <YA_DHT22_M.h>

#include "alma_clock.h"
#include "alma_math.h"
#include "sdlog.h"
#include "NVStore.h"
#include "wiring.h"

static uint32_t _nextTime = UINT_MAX;
static int _reportInterval = 10;
static uint32_t _today = 0;

void satellite_loop() {
  printf("sat %ld\t%ld\t%ld\n", _today, alma_clock, _nextTime);
  if (!_today) return; // clock not armed
  if (alma_clock < _nextTime) return;

  char buffer[128];

  int humidity_10 = -128;
  int temp_10 = -128;
  int error = DHT22_acquisition::acquire(DHT_22_1WIRE, &humidity_10, &temp_10);
  printf("error = %d\n", error);
  humidity_10 = (humidity_10 + 5) / 10;
  temp_10 = (humidity_10 + 5) / 10;
  
  snprintf(buffer, 128, "T=%d H=%d", temp_10, humidity_10);
  sdlog("satcom", buffer);
  // Sending takes place here
  if (!_nextTime)
    // First transmisstion of the day
    NV<uint32_t>::set("UlstXmt", &_today, _today);
  _nextTime = alma_clock + _reportInterval;
}

// Called only once by the virtue of alma_clock/_rtc_set

void satellite_armClock(const char *date) {
  _today = fast_atoi(date + 4) << 16 | fast_atoi(date + 2) << 8 | fast_atoi(date);
  printf("date = %lx\n", _today);
  uint32_t lastXmit = NV<uint32_t>::get("UlstXmt");
  if (lastXmit < _today) {
    _nextTime = 0; // That is now, first message of day
  } else {
    // We already sent a message today, wait for an hour. Note that if
    // we never have continuous power for _reportInterval (1 hour by
    // default), we'll send only one report per day (assuming there's
    // satellite connectivity).
    _nextTime = alma_clock + _reportInterval;
  }
  char buffer[128];
  snprintf(buffer, 128, "next xmit at %ld", _nextTime);
  sdlog("satcom", buffer);
}

void satellite_init() {
  NV<uint32_t>::setFirstValue("UlstXmt", 0);
  NV<uint16_t>::setFirstValue("uSatInt", 3600);
  _reportInterval = NV<uint16_t>::get("uSatInt");
}

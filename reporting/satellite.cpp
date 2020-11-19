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
  if (!_today) return; // clock not armed
  if (alma_clock < _nextTime) return;

  int humidity_10 = -128;
  int temp_10 = -128;
  DHT22_acquisition::acquire(DHT_22_1WIRE, &humidity_10, &temp_10);
  humidity_10 = (humidity_10 + 5) / 10;
  temp_10 = (temp_10 + 5) / 10;
  
  // Sending takes place here
  if (!_nextTime) {
    // First transmisstion of the day
    uint32_t stub = 0;
    NV<uint32_t>::set("UlstXmt", &stub, _today);
  }
  _nextTime = alma_clock + _reportInterval;

  char buffer[128];
  snprintf(buffer, 128, "T=%d H=%d N=%ld", temp_10, humidity_10, _nextTime);
  sdlog("satcom", buffer);
}

// Called only once by the virtue of alma_clock/_rtc_set

void satellite_armClock(const char *date) {
  _today = fast_atoi(date + 4) << 16 | fast_atoi(date + 2) << 8 | fast_atoi(date);
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

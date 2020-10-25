#include "alma_clock.h"
#include "alma_math.h"
#include "sdlog.h"

static uint32_t start;
uint32_t alma_clock = 0;

void alma_clock_advance() {
    alma_clock = time(NULL) - start;
}

void alma_clock_init() {
  start = time(NULL);
}

static bool _rtc_set = false;

void alma_clock_resetClock(const char *gpsDate, const char *gpsTime) {
  if (_rtc_set) return;

  struct tm _tm;
  _tm.tm_sec = fast_atoi(gpsTime + 4);
  _tm.tm_min = fast_atoi(gpsTime + 2);
  _tm.tm_hour = fast_atoi(gpsTime);
  _tm.tm_mday = fast_atoi(gpsDate);
  _tm.tm_mon = fast_atoi(gpsDate + 2) - 1;
  _tm.tm_year = 100 + fast_atoi(gpsDate + 4);

  time_t t = mktime(&_tm);
  set_time(t);
  uint32_t c = alma_clock;
  start = t - alma_clock;
  alma_clock_advance();

  char buffer[128];
  snprintf(buffer, 128, "time set to %s, delta (should be zero) = %ld", ctime(&t), alma_clock - c);
  sdlog("clock", buffer);

  _rtc_set = true;
}

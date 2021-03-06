#include <time.h>

#include "AsyncStarter.h"
#include "alma_clock.h"
#include "alma_math.h"
#include "mbed.h"
#include "nav.h"
#include "reporting.h"
#include "sdlog.h"
#include "wiring.h"

static Serial gps(GPS_TX, GPS_RX, 9600);

bool fixOk = false;

static const int maxField = 15;

static char *_message[maxField];

static void splitMessage(char *msg) {
  char **p = _message;
  *p++ = msg;
  for(;;++msg) {
    if (*msg == ',') {
      *msg = '\0';
      *p++ = msg + 1;
      if (p - _message >= maxField) return;
    } else if (*msg == '*') {
      *msg = '\0';
      return;
    } else if (!*msg) return;
  }
}

Mutex coordMutex;
float latf = 100;
float lonf = -119;

static float convertDeg(const char *p, bool _3digit, bool neg) {
  const char *pp;
  float pos;
  if (_3digit) {
    pp = p + 3;
    pos = fast_atoi(p + 1);
    if (*p == '1') pos += 100.0;
  } else {
    pp = p + 2;
    pos = fast_atoi(p);
  }
  pos += fast_atoi(pp) / 60.0;
  pp = strchr(pp + 1, '\0') - 1;
  float frac = 0;
  for(;;--pp) {
    int n = *pp - '0';
    if (n < 0 || n > 9) break;
    frac = (frac + n) / 10.0;
  }
  pos += frac / 60.0;
  if (neg) pos = -pos;
  return pos;
}

static bool dateOK(const char *year) {
  return *year != '8' || year[1] != '0'; // Year 80 when no fix yet
}

static void test_nav();

static bool processGPSMessage(char *msg) {
  sdlog("gps", msg);
  splitMessage(msg);
  if (strcmp(_message[0], "$GPRMC")) return false;

  const char *date = _message[9];
  const char *time = _message[1];

  if (*date >= '0' && *date <= '9' && dateOK(date + 4)) {
    alma_clock_resetClock(date, time);
    sdlog_checkClock(date, time);
  }

  satellite_loop(); // Called from there for now

  // test_nav();
  // return;

  fixOk = _message[2][0] == 'A';
  if (!fixOk) {
    coordMutex.lock();
    latf = 100;
    coordMutex.unlock();
    return false;
  }

  float latf_ = convertDeg(_message[3], false, _message[4][0] != 'N');
  float lonf_ = convertDeg(_message[5], true, _message[6][0] != 'E');
  coordMutex.lock();
  latf = latf_;
  lonf = lonf_;
  coordMutex.unlock();

  return true;
}

static const unsigned gpsBufferLength = 92;

static char _gpsMessage[gpsBufferLength]; // NMEA says 82

static EventFlags _e;

static int _events;

static void serialCB(int events) {
  _events = events;
  _e.set(1);
}

//static const char *debugMsg = "$GPRMC,175504.000,A,4852.6779,N,00158.5257,E,0.77,3.49,060615,,,A*6B\n";

static void globSerial() {
  printf("#\n");
  while(gps.readable()) printf("%c", gps.getc());
}

// $PMTK161,0*28 // stand by

static const char *startupMessages[] = {
  "$PMTK000*32\r\n", // Test (wake up)
  "$PMTK101*32\r\n", // Reset
  "$PMTK605*31\r\n", // Get GPS firmware reference
  "$PMTK447*35\r\n", // Get Nav threshold
  "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n", // Only GPRMC messages
  "$PMTK220,5000*1B\r\n", // Every 5s
  NULL
};

static bool gps_is_sleeping = false;

void gps_sleep() {
  if (gps_is_sleeping) return;
  gps.printf("$PMTK161,0*28\r\n");
  gps_is_sleeping = true;
}

void gps_wakeup() {
  if (!gps_is_sleeping) return;
  gps.printf("$PMTK000*32\r\n");
  gps_is_sleeping = false;
}

static void initProc() {
  globSerial();
  _gpsMessage[gpsBufferLength - 1] = '\0'; // Guardian
  gps.read((uint8_t *)_gpsMessage, gpsBufferLength - 1, serialCB, SERIAL_EVENT_RX_ALL, '\n');
  const char *const *script = startupMessages;
  gps_wakeup();
  sdlog("gps", "up");

  for(;;) {
    _e.wait_any(1);
    gps.read((uint8_t *)_gpsMessage, gpsBufferLength - 1, serialCB, SERIAL_EVENT_RX_ALL, '\n');
    if (*script) {
      gps.printf(*script);
      ++script;
    }
    if (!(_events & SERIAL_EVENT_RX_CHARACTER_MATCH)) {
      char buffer[128];
      snprintf(buffer, 128, "serial event %d", _events);
      sdlog("gpsloop", buffer);
      continue;
    }
    char *p = strchr(_gpsMessage, '\r');
    if (!p) {
	sdlog("gpsloop", "#incomplete GPS string ???");
	continue;
    }
    *p = '\0';
    bool fix = processGPSMessage(_gpsMessage);
    if (fix) bearing_loop(latf, lonf);
  }
}

static AbstractThread _t("GPS");

void gpsLoop_init() {
  _t.run(initProc);
}

////////////////////////////////////////////////////////////////////////////////

static float lac[] = {
  1.9753493563976, 48.8779951457819,
  1.97549532682324, 48.8779618769324,
  1.97566956526056, 48.8779181817373,
  1.97584744980588, 48.8778973201835,
  1.97602771175736, 48.877859574701,
  1.97624050396967, 48.8778100680465,
  1.97641423862168, 48.877758856031,
  1.97662655504995, 48.8777162945162,
  1.97688316280179, 48.8776520949852,
  1.97711396647563, 48.8776021167648,
  1.97730294325816, 48.8775563595924,
  1.97751184672879, 48.8775025140992,
  1.97756664681787, 48.8773708876219,
  1.97743276002368, 48.8772458234063,
  1.97731255565542, 48.87712982971,

  // -2.822731870550616,46.17390701395785,
  // -37.57502725067546,41.06302499630559,
  // -66.85599729490329,32.9313371535174,
  // -81.0577800180538,29.99534456797493,

  1000, 1000
};

static float *test_nav_p = lac - 1;
static int scanDirection;
static uint32_t _next_test = 0;

static void test_nav() {
  if (alma_clock < _next_test) return;

  if (test_nav_p < lac) {
    test_nav_p = lac;
    scanDirection = 2;
  }
  float _lon0 = *test_nav_p;
  if (_lon0 >= 400) {
    scanDirection = -2;
    test_nav_p -= 2;
    return;
  }

  lonf = _lon0;
  latf = test_nav_p[1];
  test_nav_p += scanDirection;
  _next_test = alma_clock + 10;
  char buffer[64];
  sprintf(buffer, "lat=%f lon=%f", latf, lonf);
  sdlog("test_nav", buffer);
}

#include <time.h>

#include "AsyncStarter.h"
#include "alma_clock.h"
#include "alma_math.h"
#include "mbed.h"
#include "nav.h"
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
float latf, lonf = 100;

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

static void processGPSMessage(char *msg) {
  sdlog("gps", msg);
  splitMessage(msg);
  if (strcmp(_message[0], "$GPRMC")) return;

  const char *date = _message[9];
  const char *time = _message[1];

  if (*date >= '0' && *date <= '9' && dateOK(date + 4)) {
    alma_clock_resetClock(date, time);
    sdlog_checkClock(date, time);
  }

  fixOk = _message[2][0] == 'A';
  if (!fixOk) {
    coordMutex.lock();
    latf = 100;
    coordMutex.unlock();
    return;
  }

  float latf_ = convertDeg(_message[3], false, _message[4][0] != 'N');
  float lonf_ = convertDeg(_message[5], true, _message[6][0] != 'E');
  coordMutex.lock();
  latf = latf_;
  lonf = lonf_;
  coordMutex.unlock();
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

static void initProc() {
  gps.printf("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n"); // Only GPRMC messages
  gps.printf("$PMTK220,5000*1B\r\n"); // Every 5s

  _gpsMessage[gpsBufferLength - 1] = '\0'; // Guardian
  gps.read((uint8_t *)_gpsMessage, gpsBufferLength - 1, serialCB, SERIAL_EVENT_RX_ALL, '\n');
  for(;;) {
    _e.wait_any(1);
    if (_events & SERIAL_EVENT_RX_CHARACTER_MATCH) {
      char *p = strchr(_gpsMessage, '\r');
      *p = '\0';
      processGPSMessage(_gpsMessage);
    }
    bearing_loop(latf, lonf);
    gps.read((uint8_t *)_gpsMessage, gpsBufferLength - 1, serialCB, SERIAL_EVENT_RX_ALL, '\n');
  }
}

static AbstractThread _t;

void gpsLoop_init() {
  _t.run(initProc);
  printf("gps up\n");
}

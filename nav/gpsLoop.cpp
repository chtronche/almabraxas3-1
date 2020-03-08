#include <time.h>

#include "AsyncStarter.h"
#include "mbed.h"
#include "nav.h"
#include "wiring.h"

static Serial gps(GPS_TX, GPS_RX, 9600);

bool fixOk = false;
uint32_t gpsClock = 0;

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

static unsigned d2(const char *p) { return (p[0] - '0') * 10 + p[1] - '0'; }

static int nbJours(int d, int m, int y) {
  m = (m + 9) % 12;
  y = y - m / 10;
  return 365*y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + ( d - 1 );
}

static uint32_t _epoch0 = nbJours(1, 1, 1970);

Mutex coordMutex;
float latf, lonf = 100;

static float convertDeg(const char *p, bool _3digit, bool neg) {
  const char *pp;
  float pos;
  if (_3digit) {
    pp = p + 3;
    pos = d2(p + 1);
    if (*p == '1') pos += 100.0;
  } else {
    pp = p + 2;
    pos = d2(p);
  }
  pos += d2(pp) / 60.0;
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

static void processGPSMessage(char *msg) {
  splitMessage(msg);
  for(int i = 0; i <= 12; i++)
    printf("%d_%s ", i, _message[i]);
  printf("\n");
  if (strcmp(_message[0], "$GPRMC")) return;
  fixOk = _message[2][0] == 'A';
  if (!fixOk) {
    coordMutex.lock();
    latf = 100;
    coordMutex.unlock();
    return;
  }

  const char *date = _message[9];
  const char *time = _message[1];
  gpsClock = ((nbJours(d2(date), d2(date + 2), 2000 + d2(date + 4)) - _epoch0) * 86400
		  + d2(time) * 3600 + d2(time + 2) * 60 + d2(time + 4));

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
      *p = '\n';
      // strcpy(_gpsMessage, debugMsg);
      printf(_gpsMessage);
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

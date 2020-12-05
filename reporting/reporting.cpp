#include <stdio.h>
#include <sys/types.h>

#include "mbed.h"

#include "alma_clock.h"
#include "alma_flags.h"
#include "alma_math.h"
#include "AsyncStarter.h"
#include "commander.h"
#include "helmsman.h"
#include "main.h"
#include "nav.h"
#include "ping.h"
#include "powerManager.h"
#include "reporting.h"
#include "sdlog.h"
#include "vars.h"

static void add8(char *&p, uint8_t v) {
  *p++ = v;
}

static void add16(char *&p, uint16_t v) {
  *p++ = (v & 0xff00) >> 8;
  *p++ = v & 0xff;
};

static void add32(char *&p, uint32_t v) {
  *p++ = (v & 0xff000000) >> 24;
  *p++ = (v & 0xff0000) >> 16;
  *p++ = (v & 0xff00) >> 8;
  *p++ = v & 0xff;
};

static uint32_t _nextClock = 0;

static char *_dump_vars(char *buffer) {
  *buffer++ = '>';
  return vars_get_random_str(buffer, 50);
}

extern void radioCheck(uint32_t clock);

static uint8_t _rssi;
static uint32_t _lat, _lon;

static const struct reportField {
  const char *prefix;
  volatile void *varp;
} frameDescription[] = {
  "4%ld", &alma_clock,

  "2V=%d", &voltage,
  "2%d", &voltageReading,
  "2I=%d", &current,
  "2%d", &currentReading,

  "2P=%d", &powerBudget,
  "1MPPT=%d", &mppt_direction,

  "2L=%d", &leftPower,
  "2R=%d", &rightPower,
  "4PP=%d", &peakPower,
  "2H=%d", &heading,
  "1MH=%d", &magneticHeading,
  "1TH=%d", &targetHeading,
  "1|=%d", &helm,
  "1%d", &forcedSteering_reverse,
  "2WP=%d", &uNavPnt,

  "l%f", &_lat,
  "l_%f", &_lon,

  "1v=%d", &_rssi,
  "2^=%d", &ping.lost,

  "4%ld", &badCommand,
  
  NULL, NULL
};

#define FORMAT_BUFFER_LEN (128)

static char _reportDescription[FORMAT_BUFFER_LEN];

// Needs flag_init

void reporting_init() {
  char *rdp = _reportDescription;
  for(const reportField *p = frameDescription; p->prefix; p++) {
    unsigned len = strlen(p->prefix);
    if (rdp + len + 1 > _reportDescription + FORMAT_BUFFER_LEN) {
      printf("reporting: report string too long, init failed\n");
      break;
    }

    strcpy(rdp, p->prefix);
    rdp += len;
    *rdp++ = ' ';
  }
  if (rdp > _reportDescription) {
    rdp[-1] = '\0';
  }
  printf("\treporting description is '%s'\n", _reportDescription);
  sdlog("up", "reporting");
}

void reporting_get_description(unsigned n) {
  if (n > 9) return; // Ask a stupid question (that will overflow the buffer), get no answer
  char buffer[60];
  const char *end = _reportDescription + strlen(_reportDescription);
  const char *p = _reportDescription + n * 54;
  if (p > end) return; // Ask a stupid question, get no answer
  sprintf(buffer, ">RD%d", n);
  unsigned left = strlen(p);
  unsigned toCopy = MIN(left, 54);
  memcpy(buffer + 4, p, toCopy);
  if (left > 54) {
    buffer[toCopy + 4] = '+';
    buffer[toCopy + 5] = '\0';
  } else {
    buffer[toCopy + 4] = '\0';
  }
  printf("%s\n", buffer);
  reporting_debug_print(buffer);
  radioSendFrame(strlen(buffer), buffer);
}

void reporting_loop() {
  processCommand(reporting_serial_read());
  processCommand(readRadioPacket());
  radioCheck(alma_clock);

  //const char *comment = "<comment>";
  _rssi = getRSSI();
  _lat = uint32_t(latf * INT_MAX / 180.0);
  _lon = uint32_t(lonf * INT_MAX / 180.0);
  if (flag_copy_radio_to_serial) {
    for(const reportField *rfp = frameDescription; rfp->prefix; rfp++) {
      const char *prefix = rfp->prefix + 1;
      switch(rfp->prefix[0]) {
      case '1':
	printf(prefix, *(uint8_t *)rfp->varp);
	break;
      case '2':
	printf(prefix, *(int16_t *)rfp->varp);
	break;
      case '4':
	printf(prefix, *(uint32_t *)rfp->varp);
	break;
      case 'l':
	printf(prefix, float(*(uint32_t *)rfp->varp) * 180.0 / INT_MAX);
	break;
      default:
	printf("?%c?", rfp->prefix[0]);
      }
      printf(rfp[1].prefix ? " " : "");
    }
    printf("\n");
    //reporting_debug_print(buffer);
  }

  if (radio_is_sleeping) return;

  char buffer[256];
  char *p = buffer;

  if (alma_clock >= _nextClock) {
    _nextClock = alma_clock + 10;
    p = _dump_vars(buffer);
  } else {
    for(const reportField *rfp = frameDescription; rfp->prefix; rfp++) {
      switch(rfp->prefix[0]) {
      case '1':
	add8(p, *(uint8_t *)rfp->varp);
	break;
      case '2':
	add16(p, *(int16_t *)rfp->varp);
	break;
      case '4':
	add32(p, *(uint32_t *)rfp->varp);
	break;
      case 'l':
	add32(p, *(uint32_t *)rfp->varp);
	break;
      default:
	printf("Radio: ?%c?", rfp->prefix[0]);
      }
    }
  }

  if (p - buffer > 60)
    sdlog("reporting", "Radio frame too long");
  else
    radioSendFrame(p - buffer, buffer);
}

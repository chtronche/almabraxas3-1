#include <stdio.h>
#include <sys/types.h>

#include "mbed.h"

#include "AsyncStarter.h"
#include "commander.h"
#include "helmsman.h"
#include "main.h"
#include "nav.h"
#include "NVStore.h"
#include "ping.h"
#include "powerManager.h"
#include "reporting.h"

bool reporting_serial_active = false;

static uint32_t _flags;

// Needs NVStore_init

void reporting_init() {
  NV<uint32_t>::get("UFlags", &_flags);
  reporting_serial_active = _flags & 1;
  printf("reporting up\n");
}

void setFlag(uint8_t flag, bool value) {
  uint32_t mask = 1 << flag;
  NV<uint32_t>::set("UFlags", &_flags, (_flags & ~mask) | (mask & uint32_t(value)));
  printf("flags = %lx\n", _flags);
}

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

void reporting_loop() {
  char buffer[256];
  processCommand(reporting_serial_read());
  processCommand(readRadioPacket());
  uint32_t clock = getClock();
  //const char *comment = "<comment>";
  int rssi = getRSSI();
  if (reporting_serial_active) {
    sprintf(buffer, "%ld V=%d %d I=%d %d P=%d MPPT=%d L=%d R=%d POW=%d PP=%d H=%d MH=%d %f_%f v=%d ^=%d WP=%d %ld",
	    clock, voltage, voltageReading,
	    current, currentReading,
	    powerBudget, mppt_direction, leftPower, rightPower,
	    voltage * current, peakPower,
	    hysteresis, magneticHeading,
	    latf, lonf,
	    rssi, ping.lost,
	    uNavPnt,
	    badCommand
	    );
    reporting_debug_print(buffer);
  }

  char *p = buffer;
  add32(p, clock); // 4
  add16(p, voltage); // 6
  add16(p, voltageReading); // 8
  add16(p, current); // 10
  add16(p, currentReading); // 12
  add16(p, powerBudget); // 14
  add16(p, mppt_direction); // 16
  add16(p, leftPower); // 18
  add16(p, rightPower); // 20
  add32(p, voltage * current); // 24
  add32(p, peakPower); // 28
  add16(p, hysteresis); // 30
  add32(p, badCommand); // 34
  add16(p, rssi); // 36
  add16(p, ping.lost); // 38
  add8(p, magneticHeading); // 39
  add32(p, uint32_t(latf * INT_MAX / 180.0)); // 43
  add32(p, uint32_t(lonf * INT_MAX / 180.0)); // 47
  radioSendFrame(p - buffer, buffer);
}

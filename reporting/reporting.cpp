#include <stdio.h>
#include <sys/types.h>

#include "mbed.h"

#include "commander.h"
#include "main.h"
#include "nav.h"
#include "ping.h"
#include "powerManager.h"
#include "reporting.h"

bool reporting_serial_active = false;

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
  const char *comment = "<comment>";
  int rssi = getRSSI();
  if (reporting_serial_active) {
    sprintf(buffer, "%ld V=%d %d I=%d %d P=%d MPPT=%d L=%d R=%d POW=%d PP=%d H=%d MH=%d %ld %s v%d %d",
	    clock, voltage, voltageReading,
	    current, currentReading,
	    powerBudget, mppt_direction, leftPower, rightPower,
	    voltage * current, peakPower,
	    hysteresis, magneticHeading,
	    badCommand, comment, rssi, ping.lost);
    reporting_debug_print(buffer);
  }

  char *p = buffer;
  add32(p, clock);
  add16(p, voltage);
  add16(p, voltageReading);
  add16(p, current);
  add16(p, currentReading);
  add16(p, powerBudget);
  add16(p, mppt_direction);
  add16(p, leftPower);
  add16(p, rightPower);
  add32(p, voltage * current);
  add32(p, peakPower);
  add16(p, hysteresis);
  add32(p, badCommand);
  add16(p, rssi);
  add16(p, ping.lost);
  add8(p, magneticHeading);
  add32(p, uint32_t(latf * INT_MAX / 180.0));
  add32(p, uint32_t(lonf * INT_MAX / 180.0));
  radioSendFrame(p - buffer, buffer);
}

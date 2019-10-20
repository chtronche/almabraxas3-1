#include <stdio.h>
#include <sys/types.h>

#include "mbed.h"

#include "commander.h"
#include "powerManager.h"
#include "reporting.h"

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
  sprintf(buffer, "%ld V=%d %d I=%d %d P=%d MPPT=%d L=%d R=%d POW=%d PP=%d H=%d %ld %s",
	  clock, voltage, voltageReading,
	  current, currentReading,
	  powerBudget, mppt_direction, leftPower, rightPower,
	  voltage * current, peakPower,
	  hysteresis,
	  badCommand, comment);
  reporting_debug_print_serial(buffer);
  
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
  add16(p, getRSSI());
  radioSendFrame(p - buffer, buffer);
}

void reporting_debug_print(const char *s) {
  reporting_debug_print_serial(s);
}


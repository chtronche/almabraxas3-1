#include <stdio.h>
#include <sys/types.h>

#include "mbed.h"
#include "powerManager.h"
#include "reporting.h"

static char buffer[256];

void reporting_loop() {
  for(;;) {
    uint32_t clock = getClock();
    const char *comment = "<comment>";
    int badCommand = -1;
    sprintf(buffer, "%ld V=%d %d I=%d %d P=%d MPPT=%d L=%d R=%d POW=%d PP=%d H=%d %ld %s",
	    clock, voltage, voltageReading,
	    current, currentReading,
	    powerBudget, mppt_direction, leftPower, rightPower,
	    voltage * current, peakPower,
	    hysteresis,
	    badCommand, comment);
    reporting_debug_print_serial(buffer);
    wait(1);
  }
}

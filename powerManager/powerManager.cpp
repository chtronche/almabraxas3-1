// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "calibration.h"
#include "LinearMapper.h"
#include "helmsman.h"
#include "nav.h"
#include "NVStore.h"
#include "powerManager.h"
#include "reporting.h"
#include "sdlog.h"
#include "vars.h"

volatile uint8_t voltage; // in V/10
volatile uint8_t current; // in A/10

volatile uint16_t powerBudget = 0; // in PWM units
static uint16_t peakPowerBudget = 0; // in PWM unit

// ============================== getPowerBudget ================================

power_t peakPower;

volatile int8_t mppt_direction = 2; // start by increasing
unsigned _hysteresis = 200; // That is 2W

static void reverseMPPTDirection() {
  mppt_direction = -mppt_direction;
  // We go back to throttle giving the best power we ever had
  // powerBudget = peakPowerBudget;
  // We reset peak power, so we'll measure power at previous peak
  // power throttle on next step.
  peakPower = peakPowerBudget = 0;
}

static uint16_t _uvPT0 = 0;

static void powerManager_getPowerBudget(uint8_t voltage, uint8_t current) {
  if (!mppt_direction) return;
  if (voltage < _uvPT0) {
    powerBudget = peakPower = peakPowerBudget = 0;
    mppt_direction = 2;
    return;
  }
  // if (current < 0.1Amp) {
  //   // Powered by capacitor, we decrease consumption until PV voltage balance capacitor voltage
  //   powerDirection = -2;
  //   return;
  // }

  if (!powerBudget) {
    // Lower bound
    // We're at a power budget so low (may be not enough light, or at
    // the beginning). We try to get more power. For now we can't tell
    // anything about max power.
    mppt_direction = 2;
    peakPower = peakPowerBudget = 0;
  } else if (powerBudget >= CALIBRATION_MAX_PWM * 2) {
    // Upper bound
    // Most likely, the load can't absorb all the power ! Which isn't
    // very good. We try reducing throttle.
    reverseMPPTDirection();
  } else {
    // Not lower nor upper bound
    power_t power = voltage * current; // in W / 100
    if (power > peakPower) {
      peakPower = power;
      peakPowerBudget = powerBudget;
    } else {
      // Hysteresis is 1.5 x V to protect from +1/-1 unit aliasing on the current reading (~100 mA)
      // At most, hysteresis is ~ 250 x 1.5 = 375, that is 3.75W, but
      // for peak power, we're around 18V, so it's ~2.7W
      if (peakPower >= _hysteresis) {
	uint16_t comp = peakPower - _hysteresis;
	if (power < comp) {
	  // We are out of the maximum power window in either direction,
	  // so we change.
	  reverseMPPTDirection();
	}
      }
    }
    // If we're here, nothing special happened, and we go on in the
    // same direction for next step.
  }
  powerBudget += mppt_direction;
}

//////////////////////////// debug ///////////////////////////////////////////////////

static const int _logBufferDepth = 256;
static FILE *_logFile = NULL;
static uint16_t _logBuffer[_logBufferDepth];
static int _logBufferIdx;

void currentSamplerLogger_start(const char *fileName) {
  if (_logFile) return;
  char buffer[128];
  snprintf(buffer, 128, "/fs/slog/%s", fileName);
  _logBufferIdx = 0;
  _logFile = fopen(buffer, "w");
}

inline void currentSamplerLogger_log(uint16_t i) {
  if (!_logFile) return;
  _logBuffer[_logBufferIdx] = i;
  if (++_logBufferIdx < _logBufferDepth) return;
  fwrite(_logBuffer, sizeof(uint16_t), _logBufferDepth, _logFile);
  _logBufferIdx = 0;
}

void currentSamplerLogger_stop() {
  if (!_logFile) return;
  if (_logBufferIdx)
    fwrite(_logBuffer, sizeof(uint16_t), _logBufferIdx, _logFile);
  fclose(_logFile);
  _logFile = NULL;
}

// ================================= Loop =============================

volatile NVLinearMapper vMapper("iv0From", "fv0To", "iv1From", "fv1To");
volatile NVLinearMapper iMapper("ii0From", "fi0To", "ii1From", "fi1To");

extern void powerManager_osd_init();

void powerManager_init() {
  powerManager_osd_init();
  _uvPT0 = NV<uint16_t>::get("uvPT0");
  vars_register("uvPT0", &_uvPT0);

  _hysteresis = NV<uint16_t>::get("uPowHys");
  vars_register("uPowHys", &_hysteresis);
  
  const_cast<NVLinearMapper &>(vMapper).retrieve();
  const_cast<NVLinearMapper &>(iMapper).retrieve();
  char buffer[128];
  sprintf(buffer, "up %f %f", iMapper.a, iMapper.b);
  mpptSwitch_init();
  sdlog("powermanager", buffer);
}

#include "adebug.h"
#include "mbed.h"

uint16_t voltageReading, currentReading;

void powerManager_loop_cb(uint16_t v, uint16_t i) {
  voltageReading = v;
  currentReading = i;

  voltage = const_cast<NVLinearMapper &>(vMapper).convert(v);
  current = const_cast<NVLinearMapper &>(iMapper).convert(i);
  
  powerManager_getPowerBudget(voltage, current);
  helmsman_dispatchPower(powerBudget, 0, 0);
  currentSamplerLogger_log(i);
}

// ================================= Harware switch  =============================

void mpptSwitch_on() {
  mppt_direction = 2;
}

void mpptSwitch_off() {
  mppt_direction = 0;
  powerBudget = 0;
}


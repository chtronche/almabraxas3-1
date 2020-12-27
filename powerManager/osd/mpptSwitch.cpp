#include <mbed.h>

#include "powerManager.h"
#include "wiring.h"

static InterruptIn _mpptSwitchInt(MPPT_SWITCH_INT);

void mpptSwitch_init() {
  _mpptSwitchInt.fall(mpptSwitch_off);
  _mpptSwitchInt.rise(mpptSwitch_on);
  // Set initial state
  if (_mpptSwitchInt)
    mpptSwitch_on();
  else
    mpptSwitch_off();
}


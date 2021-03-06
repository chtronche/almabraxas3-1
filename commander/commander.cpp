#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#include "alma_flags.h"
#include "commander.h"
#include "helmsman.h"
#include "main.h"
#include "nav.h"
#include "NVStore.h"
#include "ping.h"
#include "powerManager.h"
#include "reporting.h"
#include "sdlog.h"
#include "TokenFinder.h"
#include "vars.h"

uint32_t badCommand = 0;

static token _verbFinderData[] = {
  "set", 0x100,
  "reset", 0x200,
  "start", 0x300,
  "stop", 0x400,
  "read", 0x500,
  "convert", 0x600,
  "pi", 0x700,
  "get", 0x900,
  "dump", 0xa00,
  NULL, 0
};

static TokenFinder _verbFinder(_verbFinderData);

static token _nounFinderData[] = {
    "comment", 0x01,
    "budget", 0x02,
    "voltage", 0x4,
    "current", 0x5,
    "hysteresis", 0x6,
    "ng", 0x7,
    "helm", 0xa,
    "v0", 0xb,
    "v1", 0xc,
    "i0", 0xd,
    "i1", 0xe,
    "var", 0xf,
    "report_desc", 0x10,
    "gps", 0x11,
    "radio", 0x12,
    "reverse", 0x13,
    "debug", 0x14,
    "logISamp", 0x15,
    NULL, 0
};

static TokenFinder _nounFinder(_nounFinderData);

char comment[17];

// Syntax is verb noun with optional value

static char _x[128];

int nnn;

static void strif(const char *_next, int16_t *i, float *f) {
  char *end;
  *i = strtol(_next, &end, 10);
  *f = strtof(end, &end);
}

static bool lastWasDot = false;

void processCommand(const char *command) {
  if (!command) return;
  sdlog("command", command);

  bool bc = false;
  const char *_next;
  unsigned noun;
  unsigned verb = _verbFinder.find(command, &_next);
  if (!verb)
    bc = true;
  else {
    noun = _nounFinder.find(_next, &_next);
    if (!noun) bc = true;
  }
  if (bc) {
    ++badCommand;
    if (flag_copy_radio_to_serial) {
      sprintf(_x, ">%s", command);
      reporting_debug_print(_x);
    }
    return;
  }

  int16_t i;
  float f;
  char *p;

  if ((verb|noun) == 0x707) {
    printf(".");
    lastWasDot = true;
  } else {
    if (lastWasDot) printf("\n");
    lastWasDot = false;
    printf("command %x\n", (verb|noun));
  }
  switch(verb|noun) {
  case 0x101: // Set comment
    strncpy((char *)comment, _next, 16);
    break;

  case 0x202: // Reset budget
    mppt_direction = 2;
    break;

  case 0x102: // Set budget
    mppt_direction = 0;
    powerBudget = atoi(_next);
    break;

  // case 0x303: // start calibration
  //   powerManager_startCalibration();
  //   break;

  // case 0x403: // stop calibration
  //   powerManager_stopCalibration();
  //   break;

  // case 0x503: // read calibration
  //   powerManager_readCalibration();
  //   break;

  // case 0x104: // set 
  //   if (!powerManager_commanderSetCalibration(_next)) ++badCommand;
  //   break;

  // case 0x604: // convert voltage
  //   powerManager_commanderConvertCalibration(_next, true);
  //   break;

  // case 0x605:
  //   powerManager_commanderConvertCalibration(_next, false);
  //   break;

  // case 0x106: // set hysteresis
  //   powerManager_commanderSetHysteresis(_next);
  //   break;

  case 0x10a: // set helm
    forcedSteering = atoi(_next);
    break;

  case 0x10b: // set v0
    strif(_next, &i, &f);
    const_cast<NVLinearMapper &>(vMapper).resetPoint(i, f, false);
    break;

  case 0x10c: // set v1
    strif(_next, &i, &f);
    const_cast<NVLinearMapper &>(vMapper).resetPoint(i, f, true);
    break;

  case 0x10d: // set i0
    strif(_next, &i, &f);
    const_cast<NVLinearMapper &>(iMapper).resetPoint(i, f, false);
    break;

  case 0x10e: // set i1
    strif(_next, &i, &f);
    const_cast<NVLinearMapper &>(iMapper).resetPoint(i, f, true);
    break;

  case 0x10f: // set var
    p = strchr(_next, ' ');
    if (!p) break;
    *p = '\0';
    
    vars_set(_next, p + 1);
    break;

  case 0x311: // start gps
    gps_wakeup();
    break;

  case 0x411: // stop gps
    gps_sleep();
    break;

  case 0x212: // reset radio
    radio_reset();
    break;

  case 0x312: // start radio
    radio_wakeup();
    break;

  case 0x412: // stop radio
    radio_sleep();
    break;

  case 0x707: // ping (aka pi ng)
    ping.received(atoi(_next));
    break;

  case 0x314: // start debug
    setFlag(FLAG_COPY_RADIO_TO_SERIAL, true);
    break;
    
  case 0x414: // stop debug
    setFlag(FLAG_COPY_RADIO_TO_SERIAL, false);
    break;
    
  case 0x910: // get report_desc
    reporting_get_description(atoi(_next));
    break;

  case 0x313: // start reverse
    forcedSteering_reverse = true;
    break;

  case 0x413: // stop reverse
    forcedSteering_reverse = false;
    break;

  case 0x315: // start logISamp
    currentSamplerLogger_start(_next);
    break;

  case 0x415: //stop logISamp
    currentSamplerLogger_stop();
    break;

  case 0xa0f: // dump var
    NVStore_dump();
    break;

  default:
    ++badCommand;
  }
}

void commander_setup() {
  strcpy((char *)comment, "--");
  ((char *)comment)[16] = '\0';
}

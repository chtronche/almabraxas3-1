#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#include "commander.h"
#include "main.h"
#include "ping.h"
#include "powerManager.h"
#include "reporting.h"
#include "TokenFinder.h"

uint32_t badCommand = 0;

static token _verbFinderData[] = {
  "set", 0x100,
  "reset", 0x200,
  "start", 0x300,
  "stop", 0x400,
  "read", 0x500,
  "convert", 0x600,
  "pi", 0x700,
  NULL, 0
};

static TokenFinder _verbFinder(_verbFinderData);

static token _nounFinderData[] = {
    "comment", 0x01,
    "budget", 0x02,
    "calibration", 0x3,
    "voltage", 0x4,
    "current", 0x5,
    "hysteresis", 0x6,
    "ng", 0x7,
    NULL, 0
};

static TokenFinder _nounFinder(_nounFinderData);

static const char *startswith(const char *searched, const char *buffer) {
  for(;;) {
    if (!*searched) return buffer;
    if (!*searched) return buffer;
    if (*searched != *buffer) return NULL;
    ++searched;
    ++buffer;
  }
}

char comment[17];

// Syntax is verb noun with optional value

static char _x[128];

int nnn;

void processCommand(const char *command) {
  if (!command) return;

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
    sprintf(_x, ">%s", command);
    reporting_debug_print_serial(_x);
    return;
  }

  switch(verb|noun) {
  case 0x101: // Set comment
    strncpy((char *)comment, _next, 16);
    break;

  case 0x202: // Reset budget
    mpptOn = true;
    break;

  case 0x102: // Set budget
    mpptOn = false;
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

  // case 0x104: // set calibration
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

  case 0x707: // ping (aka pi ng)
    ping.received(atoi(_next));
    break;
    
  default:
    ++badCommand;
  }
}

void commander_setup() {
  strcpy((char *)comment, "--");
  ((char *)comment)[16] = '\0';
}

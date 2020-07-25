#include <mbed.h>

#include "FATFileSystem.h"

#include "alma_clock.h"
#include "sdlog.h"

static uint16_t annee = 2020;
static uint8_t month = 01;
static uint8_t day = 01;
static uint8_t hour = 01;
static bool mustRotate = true;

static FILE *logFile = NULL;

static uint32_t _nextTry = 0;
static uint32_t _tryDelay = 32;

static void makedirs() {
  
}

static void rotateLog() {
  if (!logFile) return;
  fclose(logFile);
  logFile = NULL;
  makedirs();
}

static void init() {
  if (logFile) return;
  if (alma_clock < _nextTry) return;
  logFile = fopen("/fs/log.txt", "w+");
  printf("sdlog init err=%d\n", logFile == NULL);
  if (logFile) {
    _nextTry = 0;
    _tryDelay = 32;
  } else {
    _tryDelay += _tryDelay;
    _nextTry = alma_clock + _tryDelay;
    printf("sdlog clock=%d retry at %d\n", alma_clock, _nextTry);
  }
}

void sdlog(const char *subsystem, const char *message) {
  init();
  if (!logFile) return;
  int err = fprintf(logFile, "%02d%02d %d %s\t%s\n", 0, 0, alma_clock, subsystem, message);
  if (err < 0) {
    fclose(logFile);
    logFile = NULL;
  }
}

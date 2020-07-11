#include <mbed.h>

#include "FATFileSystem.h"

#include "sdlog.h"

static uint8_t annee = 20; // Bug of the year 2256 in progress !
static uint8_t month = 01;
static uint8_t day = 01;
static uint8_t hour = 01;

static FILE *logFile = NULL;

static void init() {
  if (logFile) return;
  logFile = fopen("/fs/log.txt", "w+");
  printf("sdlog init err=%d\n", logFile == NULL);
}

void sdlog(const char *subsystem, const char *message) {
  init();
  if (!logFile) return;
  int err = fprintf(logFile, "%s\t%s\n", subsystem, message);
  if (err < 0) {
    fclose(logFile);
    logFile = NULL;
  }
}

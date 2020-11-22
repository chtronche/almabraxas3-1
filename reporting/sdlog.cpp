#include <mbed.h>

#include "FATFileSystem.h"

#include "alma_clock.h"
#include "alma_flags.h"
#include "alma_math.h"
#include "reporting.h"
#include "sdlog.h"

static FILE *logFile = NULL;

static uint32_t _nextTry = 0;
static uint32_t _tryDelay = 32;

static Mutex _lock;

static void _close() {
  if (!logFile) return;
  _lock.lock();
  fclose(logFile);
  logFile = NULL;
  _lock.unlock();
}

static uint8_t logDirComponent[] = { 0, 0, 0, 255 }; // year / month / day / sentinel
static uint8_t _hour = 0;

////////////////////////////////////////////////////////////////////////////////

struct _pathBuilder {
  char buffer[64];

  _pathBuilder();
  void append(unsigned);
  void strcpy(const char *);

private:
  char *bp;

};

_pathBuilder::_pathBuilder() {
  sprintf(buffer, "/fs/log");
  bp = strchr(buffer, '\0');
}

void _pathBuilder::append(unsigned n) {
  if (n > 99) n = 99;
  *bp++ = '/';
  *bp++ = '0' + n / 10;
  *bp++ = '0' + n % 10;
  *bp = '\0';
}

void _pathBuilder::strcpy(const char *s) {
  ::strcpy(bp, s);
  bp = strchr(bp, '\0');
}

////////////////////////////////////////////////////////////////////////////////

static void _findLastLogFile() {
  printf("_findLastLogFile\n");
  _pathBuilder pb;
  for(uint8_t *p = logDirComponent; *p != 255; p++) {
    printf("Trying %s\n", pb.buffer);
    DIR *dir = opendir(pb.buffer);
    if (!dir) return;
    for(;;) {
      struct dirent *ent = readdir(dir);
      if (!ent) break;
      unsigned n = atoi(ent->d_name);
      if (n > *p) *p = n;
    }
    dir->close();
    pb.append(*p);
  }

  DIR *dir = opendir(pb.buffer);
  if (!dir) return;
  for(;;) {
    struct dirent *ent = readdir(dir);
    if (!ent) break;
    unsigned n = atoi(ent->d_name);
    if (n > _hour) _hour = n;
  }
  dir->close();
}

static void _makedirs(_pathBuilder &pb) {
  mkdir(pb.buffer, 0777);
  for(const uint8_t *p = logDirComponent; *p != 255; p++) {
    pb.append(*p);
    printf("mkdir %s\n", pb.buffer);
    mkdir(pb.buffer, 0777);
  }
}

static void _openLogFile() {
  _close();
  if (!logDirComponent[0]) {
    _findLastLogFile();
  }
  _pathBuilder pb;
  _makedirs(pb);
  pb.append(_hour);
  pb.strcpy(".txt");
  printf("Opening %s\n", pb.buffer);
  logFile = fopen(pb.buffer, "w+");
}

////////////////////////////////////////////////////////////////////////////////

static void init() {
  if (logFile) return;
  if (alma_clock < _nextTry) return;
  _openLogFile();
  printf("sdlog init err=%d\n", logFile == NULL);
  if (logFile) {
    _nextTry = 0;
    _tryDelay = 32;
  } else {
    _tryDelay += _tryDelay;
    _nextTry = alma_clock + _tryDelay;
    printf("sdlog clock=%lu retry at %lu\n", alma_clock, _nextTry);
  }
}

////////////////////////////////////////////////////////////////////////////////

static bool _modified(const char *s, uint8_t *component) {
  uint8_t n = fast_atoi(s);
  if (n == *component) return false;
  *component = n;
  return true;
}

void sdlog_checkClock(const char *gpsDate, const char *gpsTime) {
  if (*gpsDate <= '0' || *gpsDate >= '9') return;

  bool modified = false;
  modified |= _modified(gpsDate + 4, logDirComponent);
  modified |= _modified(gpsDate + 2, logDirComponent + 1);
  modified |= _modified(gpsDate, logDirComponent + 2);
  modified |= _modified(gpsTime, &_hour);
  
  if (modified) _close();
}

////////////////////////////////////////////////////////////////////////////////

void sdlog(const char *subsystem, const char *message) {
  _lock.lock();
  init();
  if (logFile) {
    int err = fprintf(logFile, "%02d%02d %lu %s\t%s\n", 0, 0, alma_clock, subsystem, message);
    if (err < 0) _close();
  }
  _lock.unlock();
  if (flag_copy_sdlog_to_serial)
    printf("%lu %s\t%s\n", alma_clock, subsystem, message);
}

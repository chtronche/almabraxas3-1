#ifndef _SDLOG_H_
#define _SDLOG_H_

void sdlog_checkClock(const char *gpsDate, const char *gpsTime);
void sdlog(const char *subsystem, const char *message);

#endif // _SDLOG_H_

#include "crc.h"

uint8_t computeCRC(unsigned len, const char *pp) {
  uint8_t res = 0;
  const unsigned char *p = (const unsigned char *)pp;
  ++len;
  --p;
  while(--len) {
    res += *++p;
  }
  return res;
}

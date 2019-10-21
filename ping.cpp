#include "ping.h"

void Ping::received(uint32_t v) {
  int delta = v - last;
  if (!delta) return; // Sanity check in case of duplicate frame
  last = v;
  if (delta < 0) {
    // counter wrapped around for some reason
    return;
  }
  if (delta == 1) return;
  lost += delta - 1;
}

#include "alma_clock.h"

static uint32_t start;
uint32_t alma_clock = 0;

void alma_clock_advance() {
    alma_clock = time(NULL) - start;
}

void alma_clock_init() {
  start = time(NULL);
}

#include "mbed.h"

#include "reporting.h"

static uint32_t start;

class _init {
    _init() {
        start = time(NULL);
    }
};

uint32_t getClock() {
    return time(NULL) - start;
}

//------------------------------------
// Hyperterminal configuration
// 9600 bauds, 8-bit data, no parity
//------------------------------------

Serial pc(SERIAL_TX, SERIAL_RX);

void reporting_debug_print_serial(const char *buffer) {
  pc.printf(buffer);
  pc.printf("\n");
}

#ifndef _PING_H_
#define _PING_H_

#include <stdint.h>

class Ping {
 public:
  Ping() { reset(); last = 2147483647; }

  void reset() { lost = 0; }
  void received(uint32_t v);
  
  int16_t lost;

 private:
  uint32_t last = 0;
  
};

#endif // _PING_H_

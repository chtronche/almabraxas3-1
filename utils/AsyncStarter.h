#ifndef _ASYNC_STARTER_H_
#define _ASYNC_STARTER_H_

#include "mbed.h"

class AsyncStarter {
 public:
  AsyncStarter(void (*initProc)());
  void ready();

 private:
  struct _init *_initP;
  
};

class AbstractThread {
private:
  Thread t; // compromise with static allocation...
public:
  void run(void (*initProc)()) { t.start(initProc); }
};

#endif

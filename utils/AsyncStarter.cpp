#include "AsyncStarter.h"
#include "mbed.h"

struct _init {
  _init(void (*initProc)()) {
    t.start(initProc);
  }

  ~_init() {
    t.join();
  }

  Thread t;
};

AsyncStarter::AsyncStarter(void (*initProc)()) {
  _initP = new _init(initProc);
}

void AsyncStarter::ready() {
  if (!_initP) return;
  delete _initP;
  _initP = NULL;
}

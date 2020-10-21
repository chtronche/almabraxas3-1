#include "AsyncStarter.h"
#include "mbed.h"

struct _init {
  _init(const char *name, void (*initProc)()):t(osPriorityNormal, OS_STACK_SIZE, NULL, name) {
    t.start(initProc);
  }

  ~_init() {
    t.join();
  }

  Thread t;
};

AsyncStarter::AsyncStarter(const char *name, void (*initProc)()) {
  _initP = new _init(name, initProc);
}

void AsyncStarter::ready() {
  if (!_initP) return;
  delete _initP;
  _initP = NULL;
}

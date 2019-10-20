#ifndef _ASYNC_STARTER_H_
#define _ASYNC_STARTER_H_

class AsyncStarter {
 public:
  AsyncStarter(void (*initProc)());
  void ready();

 private:
  struct _init *_initP;
  
};

#endif

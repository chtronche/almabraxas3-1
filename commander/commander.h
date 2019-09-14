// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#ifndef _COMMANDER_H_
#define _COMMANDER_H_

#include <sys/types.h>

extern uint32_t badCommand;
extern char comment[];

void processCommand(const char *);

#endif // _COMMANDER_H_

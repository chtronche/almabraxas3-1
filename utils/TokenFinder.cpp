// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#include <ctype.h>
#include <stdio.h>

#include "reporting.h" 
#include "TokenFinder.h"

unsigned TokenFinder::find(const char *buffer, const char **next) const {
  for(;;++buffer) {
    if (!*buffer) return 0;
    if (!isspace(*buffer)) break;
  }
  for(const token *p = _list; p->token; p++) {
    const char *p0 = buffer;
    const char *p1 = p->token;
    for(;;++p0, ++p1) {
      if (!*p0 || isspace(*p0)) { // End of token
	if (*p1) break; // miss
	while(*p0 && isspace(*p0)) ++p0;
	*next = p0;
	return p->value; // Hit
      }
      if (*p0 != *p1) break; // Miss, includes *p1 == '\0'
    }
  }
  return 0; // Not found
}

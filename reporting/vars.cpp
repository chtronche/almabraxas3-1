#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include <string>
#include <vector>

#include "NVStore.h"
#include "vars.h"

typedef std::pair<const char *,void *> _cell;

static std::vector<_cell> _storage;

void vars_register(const char *key, void *v) {
  _storage.push_back(_cell(key, v));
}

static char *copyVar(char *buffer, char type_, void *p) {
  switch(type_) {
  case 'i':
    sprintf(buffer, "%d", *static_cast<int16_t *>(p));
    break;

  case 'u':
    sprintf(buffer, "%d", *static_cast<uint16_t *>(p));
    break;

  case 'U':
    sprintf(buffer, "%8lx", *static_cast<uint32_t *>(p));
    break;

  case 'f':
    sprintf(buffer, "%f", *static_cast<float *>(p));
    break;

  default:
    strcpy(buffer, "???");
  }
  return strchr(buffer, '\0');
}

char *vars_get_random_str(char *buffer, unsigned bufsize) {
  char *p = buffer;
  char *last = p;
  char *max = buffer + bufsize;
  for(;;) {
    const _cell &cell = _storage[lrand48() % _storage.size()];
    strncpy(p, cell.first, bufsize);
    p = strchr(p, '\0');
    *p++ = '\t';
    p = copyVar(p, cell.first[0], cell.second);
    if (p >= max) {
      *last = '\0';
      return last;
    }
    last = p;
    *p++ = '\t';
  }
}

static _cell *_getCell(const char *key) {
  for(std::vector<_cell>::iterator i = _storage.begin(); i != _storage.end(); ++i) {
    if (!strcmp(i->first, key)) return &(*i);
  }
  return NULL;
}

void vars_set(const char *key, const char *v) {
  _cell *p = _getCell(key);
  if (!p) return;
  void *varp = p->second;
  switch(*key) {
  case 'i':
    NV<int16_t>::set(key, static_cast<int16_t *>(varp), atoi(v));
    break;

  case 'u':
    NV<uint16_t>::set(key, static_cast<uint16_t *>(varp), strtoul(v, NULL, 10));
    break;

  case 'U':
    NV<uint32_t>::set(key, static_cast<uint32_t *>(varp), strtoul(v, NULL, 16));
    break;
    
  case 'f':
    NV<float>::set(key, static_cast<float *>(varp), atof(v));
    break;
  }
}

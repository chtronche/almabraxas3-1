// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2018 (ch@tronche.com)
// MIT License

#ifndef _TOKEN_FINDER_H_
#define _TOKEN_FINDER_H_

typedef struct { const char *token; unsigned value; } token;

class TokenFinder {
 public:
 TokenFinder(const token *list): _list(list) { } // Must be { NULL, 0 } terminated
  unsigned find(const char *str, const char **next) const;

 private:
  const token *_list;
};

#endif // _TOKEN_FINDER_H_

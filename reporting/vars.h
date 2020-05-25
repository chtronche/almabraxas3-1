// Part of the Almabraxas 3-1 project https://tronche.com/wiki/
// (c) Ch. Tronche 2020 (ch@tronche.com)
// MIT License

#ifndef _REPORTING_DICT_DUMP_H_
#define _REPORTING_DICT_DUMP_H_

void vars_init();

void vars_register(const char *key, void *v);
void vars_set(const char *key, const char *v);

char *vars_get_random_str(char *buffer, unsigned bufsize);

#endif // _REPORTING_DICT_DUMP_H_

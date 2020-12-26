#ifndef _UTILS_H_
#define _UTILS_H_

#include "bool.h"

bool cmp_str(const void *, const void *);
unsigned hash_str(const void *key);
bool cmp_int(const void *, const void *);
unsigned hash_int(const void *);

#endif

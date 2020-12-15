#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include "utils/bool.h"

void Symtable_init(int);
void Symtable_destroy(void);
void Symtable_insert(const void *);
bool Symtable_search(const void *);

#endif

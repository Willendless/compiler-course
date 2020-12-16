#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include "utils/bool.h"

typedef struct {

} SymtableEntry;

void Symtable_init(int);
void Symtable_destroy(void);
void Symtable_insert(const char *);
bool Symtable_lookup(const char *);

#endif

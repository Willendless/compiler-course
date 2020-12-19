#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include "utils/bool.h"

typedef struct {
    int n;
} SymtableEntry;

void Symtable_init(int);
void Symtable_destroy(void);
void Symtable_insert(const char *, SymtableEntry *);
SymtableEntry *Symtable_lookup(const char *);
int Symtable_length(void);
bool Symtable_exist(const char *);
void Symtable_print(void);

#endif

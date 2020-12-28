#include <stdlib.h>
#include "symtable.h"
#include "lib/set.h"
#include "lib/table.h"
#include "utils/utils.h"
#include "assert.h"
#include "utils/debug.h"

Table *symtable;

void Symtable_init(int hint) {
    if (symtable != NULL) {
        Symtable_destroy();
    }
    symtable = Table_init(hint, cmp_str, hash_str);
}

void apply_free(void *key, void **value, void *c1) {
    if (key) free(key);
    if (value) free(*value);
}

void Symtable_destroy() {
    // assert(symtable != NULL && "Try to destroy null symbol table");
    if (symtable == NULL) return;
    // clear and destroy symtable
    Table_map(symtable, &apply_free, NULL);
    Table_clear_destroy(symtable);
    symtable = NULL;
}

void Symtable_insert(const char *key, SymtableEntry *value) {
    assert(symtable != NULL && "Try to insert into NULL symbol table");
    log_info("Insert into symtable: %s", key);
    Table_put(symtable, key, value);
}

bool Symtable_exist(const char *key) {
    assert(symtable != NULL && "Try to search from NULL symbol table");
    // log_info("search symtable: %s", key);
    return Table_get(symtable, key) != NULL;
}

void Symtable_set_n(const char *key, int n) {
    SymtableEntry *en;
    if ((en = Table_get(symtable, key)) != NULL) {
        en->n = n;
    }
}

SymtableEntry *Symtable_lookup(const char *key) {
    assert(key != NULL);
    return Table_get(symtable, key);
}

int Symtable_length(void) {
    return symtable->cnt;
}

static void print_entry(const void **key, void *value, void *c1) {
    printf("symbol %d: %s\n", *(int*)c1, *key);
    *(int *)c1 += 1;
}

void Symtable_print(void) {
    int i = 1;
    Table_map(symtable, print_entry, &i);
}

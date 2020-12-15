#include <stdlib.h>
#include "symtable.h"
#include "lib/set.h"
#include "utils/utils.h"
#include "assert.h"

Set *symtable;

void Symtable_init(int hint) {
    if (symtable != NULL) {
        Symtable_destroy();
    }
    symtable = Set_init(hint, cmp_int, hash_int);
}

void Symtable_destroy() {
    assert(symtable != NULL && "Try to destroy null symbol table");

    // destroy symtable
    Set_clear_destroy(symtable);
    symtable = NULL;
}

void Symtable_insert(const void *en) {
    assert(symtable != NULL && "Try to insert into NULL symbol table");
    Set_put(symtable, en);
}

bool Symtable_search(const void *en) {
    assert(symtable != NULL && "Try to search from NULL symbol table");
    return Set_member(symtable, en);
}

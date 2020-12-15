#include <stdlib.h>
#include "symtable.h"
#include "lib/set.h"
#include "utils/utils.h"
#include "assert.h"
#include "utils/debug.h"

Set *symtable;

void Symtable_init(int hint) {
    if (symtable != NULL) {
        Symtable_destroy();
    }
    symtable = Set_init(hint, cmp_str, hash_str);
}

void apply_free(const void *mem, void *c1) {
    if (mem) free(mem);
}

void Symtable_destroy() {
    assert(symtable != NULL && "Try to destroy null symbol table");

    // clear and destroy symtable
    Set_map(symtable, apply_free, NULL);
    Set_clear_destroy(symtable);
    symtable = NULL;
}

void Symtable_insert(const void *en) {
    assert(symtable != NULL && "Try to insert into NULL symbol table");
    log_info("Insert into symtable: %s", en);
    Set_put(symtable, en);
}

bool Symtable_search(const void *en) {
    assert(symtable != NULL && "Try to search from NULL symbol table");
    log_info("search symtable: %s", en);
    return Set_member(symtable, en);
}

#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "table.h"
#include "../utils/bool.h"
#include "../utils/debug.h"

#define T Table

static int primes[] = {
    509, 509, 1021, 2053, 4093,
    8191, 16381, 32771, 65521, INT_MAX
};

T *Table_init(int hint,
    bool cmp(const void *x, const void *y),
    unsigned hash(const void *key)) {
    /* Hint used to set the capacity of the table, therefore
        should be greater than 0. */
    assert(hint >= 0);

    T *table;
    int i;

    for (i = 1; primes[i] < hint; i++) ;
    table = (T *)calloc(1, sizeof(T) + primes[i-1] * sizeof(*(table->buckets)));
    check_mem(table);
    table->bucket_size = primes[i-1];
    table->cnt = 0;
    table->cmp = cmp;
    table->hash = hash;
    table->buckets = (struct t_entry **)(table + 1);
    return table;
error:
    return NULL;
}

// static void clear_entry(const void *key, void **value, void *c1) {
//     free(*value);
// }

void Table_clear(T *table) {
    // Table_map(table, clear_entry, NULL);
    struct t_entry *p;
    struct t_entry *q;
    int i;
    assert(table != NULL && "Table should not be NULL");

    if (table->cnt == 0) return;
    for (i = 0; i < table->bucket_size; ++i) {
        for (p = table->buckets[i]; p; p = q) {
            q = p->next;
            free(q);
            table->cnt--;
        }
    } 
}

void Table_map(T *table,
    void apply(const void *key, void **value, void *c1),
    void *c1) {
    int i;
    unsigned stamp;
    struct t_entry *p;

    assert(table != NULL && "Table should not be NULL");
    assert(apply);
    stamp = table->timestamp;
    for (i = 0; i < table->cnt; ++i) {
        for (p = *(table->buckets) + i; p; p = p->next) {
            apply(p->key, p->value, c1);
            assert(table->timestamp == stamp);
        }
    }
}

void Table_destroy(T *table) {
    assert(table != NULL && "Table should not be NULL");
    free(table);
}

int Table_length(T *table) {
    return table->cnt;
}

static inline void *search_entry(T *table, const void *key) {
    struct t_entry *p;
    int index;
    assert(table != NULL);

    p = NULL;
    index = table->hash(key) % table->bucket_size;
    for (p = table->buckets[index]; p; p = p->next) {
        if (table->cmp(p->key, key) == TRUE) {
            break;
        }
    }
    return p;
}

void *Table_get (T *table, const void *key) {
    struct t_entry *p;

    assert(table != NULL && "Table should not be NULL.");
    assert(table->hash != NULL);
    assert(key != NULL && "Key should not be NULL.");

    p = search_entry(table, key);
    return p ? p->value : NULL;
}

void *Table_put(T *table, const void *key, void *value) {
    int index;
    struct t_entry *p;
    struct t_entry *en;
    void *prev;
    assert(table != NULL && "Table should not be NULL.");
    assert(key != NULL && "Key should not be NULL.");

    p = search_entry(table, key);
    if (p) {
        prev = p->value;
        p->value = value;
    } else {
        index = table->hash(key) % table->bucket_size;
        prev = NULL;
        en = (struct t_entry*)calloc(1, sizeof(struct t_entry));
        check_mem(en);
        en->key = key;
        en->value = value;

        en->next = table->buckets[index];
        table->buckets[index] = en;
        table->cnt++;
    }
    table->timestamp++;
    return prev;
error:
    return NULL;
}

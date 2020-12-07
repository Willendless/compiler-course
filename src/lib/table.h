#ifndef _TABLE_H_
#define _TABLE_H_

#include "../utils/bool.h"

typedef struct {
    int cnt;
    int bucket_size;
    bool (*cmp) (const void *a, const void *b);
    unsigned (*hash) (const void *key);
    unsigned timestamp;
    struct t_entry {
        struct t_entry *next;
        const void *key;
        void *value;
    } **buckets;
} Table;

#define T Table

T *Table_init(int hint, bool cmp(const void *x, const void *y), unsigned hash(const void *key));
void Table_clear(T *table);
void Table_destroy(T *table);
void Table_clear_destroy(T *table);
void Table_map(T *table, void apply(const void *key, void **value, void *c1), void *c1);
int Table_length(T *table);

void *Table_put(T *table, const void *key, void *value);
void *Table_get (T *table, const void *key);

#undef T
#endif

#ifndef _SET_H_
#define _SET_H_

#include "../utils/bool.h"

typedef struct {
    int cnt;    // num of members
    int bucket_size; // bucket num
    bool (*cmp) (const void *a, const void *b);
    unsigned (*hash) (const void *mem);
    struct set_entry {
        struct set_entry *next;
        const void *member;
    } **buckets;
} Set;

#define T Set

T *Set_init(int hint, bool cmp(const void *a, const void *b), unsigned hash(const void *mem));
void Set_clear(T *s);
void Set_destroy(T *s);
void Set_clear_destroy(T *s);

int Set_length(T *s);
bool Set_member(T *s, const void *mem);
int Set_put(T *s, const void *mem);
void *Set_remove(T *s, void *mem);

void Set_map(T *s, void apply(const void *mem, void *c1), void *c1);

T *Set_union(T *s, T *t);

#undef T
#endif

#include "set.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "../utils/debug.h"

#define T Set

static const int primes[] = {509, 509, 1021, 2053, 4093, 8191, 16381, 32771, 65521, INT_MAX};

T *Set_init(int hint,
    bool cmp(const void *a, const void *b), unsigned hash(const void *mem)) {
        T *set; 
        int i;
        assert(cmp != NULL);
        assert(hash != NULL);
        assert(hint >= 0);

        for (i = 1; primes[i] < hint; ++i) ;

        set = (T *)calloc(1, sizeof(T) + primes[i-1] * sizeof(struct set_entry));
        check_mem(set);
        set->cnt = 0;
        set->bucket_size = primes[i-1];
        set->cmp = cmp;
        set->hash = hash;
        set->buckets = (struct set_entry **)(set + 1);
        return set;
error:
        return NULL;
}

void Set_clear(T *s) {
    int i;
    struct set_entry *p;
    struct set_entry *q;

    assert(s != NULL);
    if (s->cnt == 0) return;
    for (i = 0; i < s->bucket_size; ++i) {
        for (p = s->buckets[i]; p; p = q) {
            q = p->next;
            free(q);
        }
    } 
}

void Set_destroy(T *s) {
    assert(s != NULL);
    free(s);
}

void Set_clear_destroy(T *s) {
    assert(s != NULL);
    Set_clear(s);
    Set_destroy(s);
}

int Set_length(T *s) {
    assert(s != NULL);
    return s->cnt;
}

bool Set_member(T *s, const void *mem) {
    unsigned int index;
    struct set_entry *en;

    assert(s != NULL);
    assert(mem != NULL);

    index = s->hash(mem) % s->bucket_size;
    for (en = s->buckets[index]; en; en = en->next) {
        if (s->cmp(mem, en->member) == TRUE) {
            return TRUE;
        }
    }
    return FALSE;
}

int Set_put(T *s, const void *mem) {
    unsigned int index;
    struct set_entry *en;
    assert(s != NULL);
    assert(mem != NULL);

    if (!Set_member(s, mem)) {
        index = s->hash(mem) % s->bucket_size;
        en = (struct set_entry *)calloc(1, sizeof(struct set_entry));
        check_mem(en);
        en->member = mem;
        en->next = s->buckets[index];
        s->buckets[index] = en;
        s->cnt++;
    }

    return 0;
error:
    return -1;
}

void *Set_remove(T *s, void *mem) {
    int index;
    struct set_entry **pp;
    struct set_entry *p;
    assert(s != NULL);
    assert(mem != NULL);

    index = s->hash(mem) % s->bucket_size;
    for (pp = &s->buckets[index]; *pp; pp = &(*pp)->next) {
        if (s->cmp(mem, (*pp)->member) == TRUE) {
            p = *pp;
            *pp = (p)->next;
            free(p);
            s->cnt--;
            return mem;
        }
    }
    return NULL;
}

void Set_map(T *s, void apply(const void *mem, void *c1), void *c1) {
    int i;
    struct set_entry *p;
    assert(s != NULL && "Table should not be NULL");
    assert(apply);
    for (i = 0; i < s->bucket_size; ++i) {
        for (p = (*(s->buckets + i)); p; p = p->next) {
            apply(p->member, c1);
        }
    }
}

void apply_union(const void *mem, void *c1) {
    T *s;
    assert(mem != NULL);
    assert(c1 != NULL);
    s = (T *)c1;
    Set_put(s, mem);
}

T *Set_union(T *s, T *t) {
    assert(s != NULL);
    assert(t != NULL);

    Set_map(t, apply_union, s);
    return s;
}

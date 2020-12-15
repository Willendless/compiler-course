#include "utils.h"
#include "bool.h"

bool cmp_str(const void *a, const void *b) {
    return !strcmp(a, b) ? TRUE : FALSE;
}

unsigned hash_str(const void *key) {
    unsigned hash = 2166136261u;
    char *p = key;

    for (; *p != '\0'; p++) {
        hash ^= *p;
        hash *= 16777619;
    }
    return hash;
}

bool cmp_int(const void *a, const void *b) {
    return a == b;
}

unsigned hash_int(const void *key) {
    return (unsigned long)key >> 2;
}

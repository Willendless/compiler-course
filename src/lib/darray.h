#ifndef _DARRAY_H_
#define _DARRAY_H_

#include <stdlib.h>
#include <assert.h>
#include "../utils/debug.h"

typedef struct {
    int end;                // end index
    int max;                // currenct maximum memory of the dynamic array, may be expanded by push
    size_t element_size;    // size of element in the array
    void **contents;        // contents array
} DArray;

#define T DArray

DArray *DArray_init(size_t element_size, size_t initial_max);
void DArray_destroy(T *arr);
void DArray_clear(T *arr);
void DArray_clear_destroy(T *arr);

int DArray_expand(T *arr);
int DArray_contract(T *arr);

int DArray_push(T *arr, void *e1);
void *DArray_pop(T *arr);

#define DArray_last(A) ((A)->contents[A.end - 1])
#define DArray_first(A) ((A)->contents[0])
#define DArray_end(A) ((A)->end)
#define DArray_count(A) DArray_end(A)
#define DArray_max(A) ((A)->max)

#define DEFAULT_EXPAND_RATE 300

static inline void *DArray_new(T *arr) {
    check(arr->element_size > 0,
            "Cannot use DArray_new on 0 size darrays.");
    
    return calloc(1, arr->element_size);
error:
    return NULL;
}

static inline void DArray_set(T *arr, int i, void *e) {
    assert(arr != NULL && "DArray pointer should not be null.");
    check(i < arr->max, "darry attempt to set beyond max.");
    if (i > arr->end)
        arr->end = i;
    arr->contents[i] = e;
    return;
error:
    return;
}

static inline void *DArray_get(T *arr, int i) {
    assert(arr != NULL && "DArray pointer should not be null.");
    check(i < arr->max, "darry attempt to get beyond max.");
    return arr->contents[i];
error:
    return NULL;
}

static inline void *DArray_remove(T *arr, int i) {
    void *ret;
    assert(arr != NULL && "DArray pointer should not be null.");
    check(i < arr->max && i >= 0, "darry attempt to remove element beyond max: %d, i: %d.", arr->max, i);
    ret = arr->contents[i];
    arr->contents[i] = NULL;
    return ret;
error:
    return NULL;
}

#define DArray_free(E) free((E))
#endif

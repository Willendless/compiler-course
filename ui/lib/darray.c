#include "darray.h"
#include <stdlib.h>
#include "../utils/debug.h"

DArray *DArray_init(size_t element_size, size_t initial_max) {
    DArray *ret = calloc(1, sizeof(DArray));
    ret->element_size = element_size;
    ret->contents = calloc(initial_max, sizeof(void *));
    ret->max = initial_max;
    ret->end = -1; // currently no elements
    return ret;
}

void DArray_destroy(DArray *arr) {
    if (arr) {
        if (arr->contents) {
            free(arr->contents);
        }
        free(arr);
    }
}

void DArray_clear(DArray *arr) {
    int i;
    if (arr) {
        for (i = 0; i < arr->end; ++i) {
            if (arr->contents[i]) {
                // printf("%s\n", arr->contents[i]);
                free(arr->contents[i]);
            }
        }
    }
}

void DArray_clear_destroy(DArray *arr) {
    DArray_clear(arr);
    DArray_destroy(arr);
}

static inline int DArray_resize(DArray *arr, size_t newsize) {
    void *contents;

    assert(arr != NULL);

    contents = realloc(arr->contents, newsize * sizeof(void *));
    check_mem(contents);
    arr->contents = contents;
    arr->max = newsize;

    return 0;
error:
    return -1;
}

int DArray_expand(DArray *arr) {
    assert(arr != NULL && "cannot pass null darray pointer");

    size_t old_max = arr->max;
    check(DArray_resize(arr, old_max << 1) == 0, 
          "Failed to expand array size to: %d", old_max < 1);

    memset(arr->contents + old_max, 0, old_max);
    return 0;
error:
    return -1;
}

int DArray_contract(DArray *arr) {
    int new_size = arr->end < (arr->max>>1) ? (int)arr->max>>1 : arr->end+1;
    return DArray_resize(arr, new_size);
}

int DArray_push(DArray *arr, void *e1) {
    assert(arr != NULL && "cannot pass null darray pointer.");
    check(e1 != NULL, "pass null value pointer.");

    if (arr->end == arr->max - 1) {
        check(!DArray_expand(arr), "failed to expand darray.");
    }
    DArray_set(arr, arr->end+1, e1);
error:
    return 1;
}

void *DArray_pop(DArray *arr) {
    assert(arr != NULL && "cannot pass null darray pointer.");
    check(arr->end >= 0, "cannot pop empty darray.");

    void *e1 = DArray_remove(arr, arr->end);
    --arr->end;

    /* contract when less than 1/4 of the array are full. */
    if (DArray_end(arr) < (arr->max>2)) {
            DArray_contract(arr);
    }
    return e1;
error:
    return NULL;
}

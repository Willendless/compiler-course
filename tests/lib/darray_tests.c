#include "../minunit.h"
#include <stdio.h>
#include <lib/darray.h>

static DArray *arr = NULL;
static int *val1 = NULL;
static int *val2 = NULL;

static char *test_init() {
    arr = DArray_init(sizeof(int), 100);
    mu_assert(arr != NULL, "DArray_create failed.");
    mu_assert(arr->contents != NULL, "contents are wrong in darry.");
    mu_assert(arr->element_size == sizeof(int), "element size is wrong.");
    mu_assert(arr->end == -1, "end isn't at the right spot.")
    mu_assert(arr->max == 100, "max is wrong.");
    return NULL;
}

static char *test_destroy() {
    DArray_destroy(arr);
    return NULL;
}

static char *test_new() {
    arr = DArray_init(sizeof(int), 100);
    val1 = DArray_new(arr);
    mu_assert(val1 != NULL, "failed to make a new element.");
    val2 = DArray_new(arr);
    mu_assert(val1 != NULL, "failed to make a new element.");
    return NULL;
}

static char *test_set() {
    DArray_set(arr, 0, val1);
    DArray_set(arr, 1, val2);
    return NULL;
}

static char *test_get() {
    mu_assert(DArray_get(arr, 0) == val1, "failed to retrieve the first element");
    mu_assert(DArray_get(arr, 1) == val2, "failed to retrieve the second element");
    return NULL;
}

static char *test_remove() {
    int *val_check;
    mu_assert(DArray_remove(arr, 100) == NULL, "failed to prevent remove nonexistent element")
    mu_assert(DArray_remove(arr, 101) == NULL, "failed to prevent remove nonexistent element")
    mu_assert(DArray_remove(arr, -1) == NULL, "failed to prevent remove nonexistent index: -1")

    val_check = DArray_remove(arr, 0);
    mu_assert(val_check != NULL, "should not get NULL");
    mu_assert(*val_check == *val1, "failed to get the first value");
    mu_assert(DArray_get(arr, 0) == NULL, "failed to get NULL after removing");

    val_check = DArray_remove(arr, 1);
    mu_assert(val_check != NULL, "should not get NULL");
    mu_assert(*val_check == *val2, "failed to get the first value");
    mu_assert(DArray_get(arr, 1) == NULL, "failed to get NULL after removing");

    DArray_free(val_check);
    return NULL;
}

static char *test_expand_contract() {
    int old_max = arr->max;
    DArray_expand(arr);
    mu_assert(arr->max == (old_max << 1), "Wrong size after expand.");
    DArray_contract(arr);
    mu_assert(arr->max == old_max, "failed to stay expand_rate at least.");
    int *val = DArray_new(arr);
    *val = 121212;
    DArray_set(arr, old_max - 1, val);
    mu_assert(DArray_get(arr, old_max - 1) == val, "failed to set at margin.");
    DArray_contract(arr);
    mu_assert(arr->max == old_max, "failed to stay old_max at least.");
    return NULL;
}

static char *test_push_pop() {
    int i;
    for (i = 0; i < 1000; ++i) {
        int *val = DArray_new(arr);
        *val = i * 333;
        DArray_push(arr, val);
    }
    mu_assert(arr->max == 1600, "wrong max size.");
    for (i = 999; i >= 0; --i) {
        int *val = DArray_pop(arr);
        mu_assert(val != NULL, "shouldn't get a NULL.");
        mu_assert(*val == i * 333, "wrong value.");
        DArray_free(val);
    }
    DArray_clear_destroy(arr);
    return NULL;
}

static char *all_tests() {
    mu_suite_start();
    mu_run_test(test_init);
    mu_run_test(test_destroy);
    mu_run_test(test_new);
    mu_run_test(test_set);
    mu_run_test(test_get);
    mu_run_test(test_remove);
    mu_run_test(test_expand_contract);
    mu_run_test(test_push_pop);
    return NULL;
}

RUN_TESTS(all_tests);

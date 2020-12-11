#include <lib/table.h>
#include "../minunit.h"

#define T Table

T *t;

static bool cmp(const void *a, const void *b) {
    return strcmp(a, b) == 0;
}

static unsigned hash(const void *key) {
    return (unsigned long)key>>2;
}

static char *test_init() {
    t = Table_init(500, cmp, hash);
    mu_assert(t != NULL, "Set_init failed.");
    mu_assert(t->bucket_size == 509, "Set_init bucket size is wrong.");
    mu_assert(t->buckets != NULL, "Set_init buckets failed.");
    mu_assert(t->cnt == 0, "Set_init cnt equals 0.");
    mu_assert(t->hash != NULL && t->cmp != NULL, "Set_init hash & cmp cannot be NULL.");
    return NULL;
}

char *k1 = "foo";
char *v1 = "bar";
char *k2 = "abc";
char *v2 = "def";

static char *test_put() {
    Table_put(t, k1, v1);
    Table_put(t, k2, v2);
    return NULL;
}

static char *test_get() {
    mu_assert(Table_get(t, k1) == v1, "Failed to put and get.");
    mu_assert(Table_get(t, k2) == v2, "Failed to put and get.");
    return NULL;
}

static char *test_size() {
    mu_assert(Table_length(t) == 2, "Failed to record table length.");
    return NULL;
}

static char *test_destory() {
    Table_destroy(t);
    return NULL;
}

static char *test_all() {
    mu_suite_start();
    mu_run_test(test_init);
    mu_run_test(test_put);
    mu_run_test(test_get);
    mu_run_test(test_size);
    mu_run_test(test_destory);
    return NULL;
}

RUN_TESTS(test_all);

#include "../minunit.h"
#include <lib/set.h>
#include <utils/bool.h>
#include <string.h>

Set *s;
Set *a;

static bool cmp(const void *a, const void *b) {
    return strcmp(a, b) == 0;
}

static unsigned hash(const void *mem) {
    return (unsigned long)mem>>2;
}

static char *test_init() {
    s = Set_init(500, cmp, hash);
    mu_assert(s != NULL, "Set_init failed.");
    mu_assert(s->bucket_size == 73, "Set_init bucket size is wrong.");
    mu_assert(s->buckets != NULL, "Set_init buckets failed.");
    mu_assert(s->cnt == 0, "Set_init cnt equals 0.");
    mu_assert(s->hash != NULL && s->cmp != NULL, "Set_init hash & cmp cannot be NULL.");
    return NULL;
}

static char *test_put() {
    Set_put(s, "abc");
    mu_assert(Set_member(s, "abc"), "Failed to put abc");
    mu_assert(Set_length(s) == 1, "Failed to record set length");
    Set_put(s, "def");
    mu_assert(Set_member(s, "def"), "Failed to put def");
    mu_assert(Set_length(s) == 2, "Failed to record set length");
    return NULL;
}

static char *test_remove() {
    Set_remove(s, "abc");
    mu_assert(Set_member(s, "abc") == FALSE, "Failed to remove abc");
    mu_assert(Set_length(s) == 1, "Failed to record set length");
    Set_remove(s, "def");
    mu_assert(Set_member(s, "def") == FALSE, "Failed to remove def");
    mu_assert(Set_length(s) == 0, "Failed to record set length");
    return NULL;
}

static char *test_union() {
    a = Set_init(500, cmp, hash);
    Set_put(a, "123");
    mu_assert(Set_length(a) == 1, "Failed to record set length");
    Set_put(s, "456");
    mu_assert(Set_length(s) == 1, "Failed to record set length");
    Set_union(s, a);
    mu_assert(Set_length(s) == 2, "Failed to union");
    mu_assert(Set_member(s, "123"), "Failed to union new element");
    mu_assert(Set_member(s, "456"), "Failed to reserve old element");
    return NULL;
}

static char *test_destroy() {
    Set_clear_destroy(s);
    Set_clear_destroy(a);
    return NULL;
}

static char *all_tests() {
    mu_suite_start();
    mu_run_test(test_init);
    mu_run_test(test_put);
    mu_run_test(test_remove);
    mu_run_test(test_union);
    mu_run_test(test_destroy);
    return NULL;
}

RUN_TESTS(all_tests);

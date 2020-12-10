#include <lib/stack.h>
#include "../minunit.h"


static Stack *s = NULL;
static char *s1 = "abc";
static char *s2 = "def";

static char *test_init() {
    s = Stack_init();
    mu_assert(s != NULL, "Stack_init failed.");
    mu_assert(s->cnt == 0, "Count of Stack not equal to zero.");
    return NULL;
}


static char *test_push() {
    // push abc
    Stack_push(s, s1);
    mu_assert(s->cnt == 1, "Count of Stack not equal to 1");
    // push def
    Stack_push(s, s2);
    mu_assert(s->cnt == 2, "Count of Stack not equal to 2");
    return NULL;
}

static char *test_pop() {
    // pop def
    char *v = Stack_pop(s);
    mu_assert(s->cnt == 1, "Count of Stack not equal to 1");
    mu_assert(!strcmp(v, s2), "Pop element not equal to abc");
    // pop abc
    v = Stack_pop(s);
    mu_assert(s->cnt == 0, "Count of Stack not equal to 1");
    mu_assert(!strcmp(v, s1), "Pop element not equal to abc");
    return NULL;
}

static char *all_tests() {
    mu_suite_start();
    mu_run_test(test_init);
    mu_run_test(test_push);
    mu_run_test(test_pop);
    return NULL;
}

RUN_TESTS(all_tests);

#include "minunit.h"
// #include <toy/scanner.h>
#include <assert.h>

char *test_dummy() {
    mu_assert(1 == 1, "Dummy test failed!");
    return NULL;
}

char *all_test() {
    mu_suite_start();
    mu_run_test(test_dummy);
    return NULL;
}

RUN_TESTS(all_test);

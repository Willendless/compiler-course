#undef NDEBUG
#ifndef _minunit_h
#define _minunit_h

#include <stdio.h>
#include <stdlib.h>
#include <utils/debug.h>

#define mu_suite_start() char *message = NULL

#define mu_assert(test, message) do {if (!(test)) {\
    log_err(message); return message;}} while(0);
#define mu_run_test(test) do {debug("\n-----%s", " " #test); \
    message = test(); tests_run++; if (message) return message;} while(0);

#define RUN_TESTS(name) int main(int argc, char *argv[]) {\
    debug("----- RUNNING %s:%d", argv[0], argc);\
    printf("----\nRUNNING: %s\n", argv[0]);\
    char *result = name();\
    printf("Tests run: %d\n", tests_run);\
    if (result != 0) {\
        printf(">> %s\n", result);\
    }\
    exit(result != 0);\
}

int tests_run;

#endif

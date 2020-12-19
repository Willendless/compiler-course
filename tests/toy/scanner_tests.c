#include "../minunit.h"
#include <assert.h>
#include <toy/scanner.h>
#include <toy/token.h>

#define T Token
#define TT TokenType

char *test_number() {
    const char *number1 = "1212";
    const char *number2 = "1212E+2";
    const char *number3 = "1212e-2";
    const char *number4 = "1212.2";
    const char *number5 = "1212.2E+3";
    const char *number6 = "1212.2E-3";
    const char *number7 = "1212.2e-3";
    T t;
    
    init_scanner(number1);
    t = scan_token();
    mu_assert(t.length == 4, "Failed to scan token length");
    mu_assert(t.type == T_NUMBER, "Failed to scan token type");
    init_scanner(number2);
    t = scan_token();
    mu_assert(t.length == 7, "Failed to scan token length");
    mu_assert(t.type == T_NUMBER, "Failed to scan token type"); 
    init_scanner(number3);
    t = scan_token();
    mu_assert(t.length == 7, "Failed to scan token length");
    mu_assert(t.type == T_NUMBER, "Failed to scan token type"); 
    init_scanner(number4);
    t = scan_token();
    mu_assert(t.length == 6, "Failed to scan token length");
    mu_assert(t.type == T_NUMBER, "Failed to scan token type"); 
    init_scanner(number5);
    t = scan_token();
    mu_assert(t.length == 9, "Failed to scan token length");
    mu_assert(t.type == T_NUMBER, "Failed to scan token type"); 
    init_scanner(number6);
    t = scan_token();
    mu_assert(t.length == 9, "Failed to scan token length");
    mu_assert(t.type == T_NUMBER, "Failed to scan token type"); 
    init_scanner(number7);
    t = scan_token();
    mu_assert(t.length == 9, "Failed to scan token length");
    mu_assert(t.type == T_NUMBER, "Failed to scan token type"); 
    return NULL;
}

char *test_comment() {
    const char *comment = "// asdfasdf";
    init_scanner(comment);
    T t = scan_token();
    mu_assert(t.type == T_EOF, "Failed to scan comment");
    return NULL;
}

char *all_test() {
    mu_suite_start();
    mu_run_test(test_number);
    mu_run_test(test_comment);
    return NULL;
}

RUN_TESTS(all_test)

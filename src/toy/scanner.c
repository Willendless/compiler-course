#include <stdio.h>
#include "scanner.h"

#define T Token
#define TT TokenType

typedef struct {
    const char *start; // current beginning of the lexme
    const char *cur; // current character
    int line;
} Scanner;

Scanner scanner;

void init_scanner(const char *source) {
    scanner.start = source;
    scanner.cur = source;
    scanner.line = 1;
}

T scan_token() {
    // TODO(ljr): complete scanner
    T t;
    t.type = T_BANG_EQUAL;
    return t;
}

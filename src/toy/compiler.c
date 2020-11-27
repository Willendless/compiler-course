#include <stdio.h>
#include "compiler.h"
#include "scanner.h"
#include "token.h"

#define T Token

void compile(const char *source) {
    int line = 0;

    init_scanner(source);
    while (1) {
        T token = scan_token();
        if (line != token.line_num) {
            printf("%4d ", token.line_num);
            line = token.line_num;
        } else {
            printf("   | ");
        }
        printf("%-14s '%.*s'\n", token_type_string[token.type], token.length, token.start);

        if (token.type == T_EOF) {
            break;
        }
        // TODO(ljr): rm
        break;
    }

}

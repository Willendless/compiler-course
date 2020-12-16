#include <stdio.h>
#include "compiler.h"
#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "utils/bool.h"
#include "ast.h"
#include "semantic_analysis.h"

#define T Token

void compile(const char *source) {
    AstNode *parse_tree;
    AstNode *ast_tree;

    // while (TRUE) {
    //     T token = scan_token();
    //     if (line != token.line_num) {
    //         printf("%4d ", token.line_num);
    //         line = token.line_num;
    //     } else {
    //         printf("   | ");
    //     }
    //     printf("%-14s '%.*s'\n", token_type_string[token.type], token.length, token.start);

    //     if (token.type == T_EOF) {
    //         break;
    //     }
    //     // TODO(ljr): rm
    //     // break;
    // }
    parse_tree = parse(source);
    AstNode_print(parse_tree, 1);
    if (parse_tree != NULL && !semantic_analysis(parse_tree)) {
        // code gen
        ast_tree = AstNode_gen_ast(parse_tree);
        AstNode_print(ast_tree, 1);
        // vm run
    }
}

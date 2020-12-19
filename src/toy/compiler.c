#include <stdio.h>
#include "compiler.h"
#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "utils/bool.h"
#include "ast.h"
#include "semantic_analysis.h"
#include "code_gen.h"
#include "vm.h"
#include "symtable.h"

#define T Token

void compile(const char *source) {
    int line = 0;
    AstNode *parse_tree;
    AstNode *ast_tree;
    DArray *code;

    // init_scanner(source);

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



    printf("%s\n", source);
    parse_tree = parse(source);
    printf("---------parse tree output-------\n");
    AstNode_print(parse_tree, 1);
    if (parse_tree != NULL && !semantic_analysis(parse_tree)) {
        printf("---------symbol table output---\n");
        Symtable_print();
        // code gen
        printf("---------ast tree output-------\n");
        ast_tree = AstNode_gen_ast(parse_tree);
        AstNode_print(ast_tree, 1);
        printf("---------code generation-------\n");
        code = code_generation(ast_tree);
        // vm run
        printf("---------vm output-------------\n");
        vm_run(code);
        // release mem
        // AstNode_clear(root);
        // DArray_clear_destroy(code);
        // symtable_destroy();
    }
}

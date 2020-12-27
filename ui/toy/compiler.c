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
#include "utils/debug.h"

#define T Token

char compile_output[2048];
bool COMPILE_ERROR = FALSE;
DArray *code = NULL;

FILE *parse_file;
FILE *ast_file;
FILE *ir_file;

static void init_compiler() {
    const char *parsef_name = ".parsef.ir";
    const char *astf_name = ".ast.ir";
    const char *irf_name = ".ir.ir";
    parse_file = fopen(parsef_name, "w+");
    assert(parse_file!=NULL && "Failed to open parse file.");
    ast_file = fopen(astf_name, "w+");
    assert(ast_file!=NULL && "Failed to open ast file.");
    ir_file = fopen(irf_name, "w+");
    assert(ir_file!=NULL && "Failed to open ir file.");
    COMPILE_ERROR = FALSE;
}

static void close_compiler() {
    fclose(parse_file);
    fclose(ast_file);
    fclose(ir_file);
}

void compile(const char *source) {
    AstNode *parse_tree;
    AstNode *ast_tree;

    init_compiler();
    parse_tree = parse(source);
    log_info("---------parse tree output-------\n");
    AstNode_file_print(parse_file, parse_tree, 1);
    if (!COMPILE_ERROR && !semantic_analysis(parse_tree)) {
        strcat(compile_output, "compile success.");
        if (parse_tree != NULL) {
            // log_info("---------symbol table output---\n");
            // Symtable_print();
            // code gen
            // log_info("---------ast tree output-------\n");
            ast_tree = AstNode_gen_ast(parse_tree);
            AstNode_file_print(ast_file, ast_tree, 1);
            // printf("---------code generation-------\n");
            code = code_generation(ast_tree, ir_file);
            // vm run
            // log_info("---------vm output-------------\n");
            // vm_run(code);
            // release mem
            // AstNode_clear(root);
            // DArray_clear_destroy(code);
            // symtable_destroy();
        }
    } else {
        strcat(compile_output, "compile failed.");
    }
    close_compiler();
}

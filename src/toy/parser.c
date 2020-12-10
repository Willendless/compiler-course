#include <stdio.h>
#include "parser.h"
#include "scanner.h"
#include "ast.h"
#include "lib/stack.h"
#include "utils/bool.h"

#define T Token

#define eof 26
// name of nonterminal
const char *NONTERMINAL_NAME[14] = {
"program",
"stmt",
"compoundstmt",
"stmts",
"ifstmt",
"whilestmt",
"assgstmt",
"boolexpr",
"boolop",
"arithexpr",
"arithexprprime",
"multexpr",
"multexprprime",
"simpleexpr",
};
// ll(1) parser table
const int LL_PARSE_TABLE[14][28] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 2, 0, 0, 3, 4, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 7, 0, 0, 7, 7, 0, 0, 0, 7, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8},
    {0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 12, 12, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 13, 15, 14, 16, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 18, 18, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 21, 19, 20, 0, 0, 0, 21, 21, 21, 21, 21, 0, 21},
    {0, 0, 0, 0, 0, 0, 0, 22, 22, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 25, 25, 25, 23, 24, 0, 25, 25, 25, 25, 25, 0, 25},
    {0, 0, 0, 0, 0, 0, 0, 26, 27, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
// begin index of nonterminal
const int non_index = 28;
// production table
const int productions[28][8] = {
    {30}, 
    {32}, {33}, {34},{30}, 
    {11, 31, 12}, 
    {29, 31}, {27}, 
    {3, 9, 35, 10, 4, 29, 5, 29}, 
    {6, 9, 35, 10, 29}, 
    {7, 20, 37, 15}, 
    {37, 36, 37}, 
    {22}, {24}, {23},{25}, {21}, 
    {39, 38}, 
    {16, 39, 38}, {17, 39, 38}, {27},
    {41, 40}, 
    {18, 41, 40}, {19, 41, 40}, {27},
    {7}, {8}, {9, 37, 10},
};

AstNode *parse(void) {
    T t = scan_token();
    Stack *stack = Stack_init();
    Stack *ast_stack = Stack_init();
    NonterminalType subtype = {STMT_PROGRAM};
    AstNode *root = AstNode_init(AST_Stmt, subtype, NULL, NONTERMINAL_NAME[0]);
    void *focus;
    // init stack
    Stack_push(stack, eof);
    Stack_push(ast_stack, root);
    focus = Stack_top(stack);
    while (TRUE) {
        // TODO(ljr): finish parser

    }
    return root;
}

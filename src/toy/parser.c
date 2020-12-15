#include <stdio.h>
#include "parser.h"
#include "scanner.h"
#include "ast.h"
#include "lib/stack.h"
#include "utils/bool.h"
#include "token.h"
#include "compiler.h"

#define T Token

#define eof 27
#define epsilon 28
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
const int LL_PARSE_TABLE[14][29] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 2, 0, 0, 3, 4, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 7, 0, 0, 7, 7, 0, 0, 0, 7, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8},
    {0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 12, 12, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 13, 15, 14, 16, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 18, 18, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 21, 19, 20, 0, 0, 0, 21, 21, 21, 21, 21, 0, 0, 21},
    {0, 0, 0, 0, 0, 0, 0, 22, 22, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 25, 25, 25, 23, 24, 0, 25, 25, 25, 25, 25, 0, 0, 25},
    {0, 0, 0, 0, 0, 0, 0, 26, 27, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
// begin index of nonterminal
const int non_index = 29;
// production table
const int productions[29][8] = {
    {},
    {31}, 
    {33}, {34}, {35},{31}, 
    {11, 32, 12}, 
    {30, 32}, {28}, 
    {3, 9, 36, 10, 4, 30, 5, 30}, 
    {6, 9, 36, 10, 30}, 
    {7, 20, 38, 15}, 
    {38, 37, 38}, 
    {22}, {24}, {23},{25}, {21}, 
    {40, 39}, 
    {16, 40, 39}, {17, 40, 39}, {28},
    {42, 41}, 
    {18, 42, 41}, {19, 42, 41}, {28},
    {7}, {8}, {9, 38, 10},
};
// snychronized set
const int SYNCHRONIZED_SET[14][29] = {
{11 ,27},
{3 ,5 ,7 ,6 ,11 ,12},
{3 ,7 ,6 ,5 ,11 ,12 ,27},
{3 ,6 ,7 ,11 ,12 ,28},
{3 ,7 ,6 ,5 ,11 ,12},
{3 ,7 ,5 ,6 ,11 ,12},
{3 ,6 ,5 ,7 ,11 ,12},
{7 ,10 ,8 ,9},
{7 ,8 ,9 ,21 ,23 ,22 ,25 ,24},
{7 ,10 ,9 ,8 ,15 ,22 ,23 ,21 ,24 ,25},
{10 ,15 ,17 ,16 ,22 ,23 ,21 ,24 ,25 ,28},
{7 ,10 ,8 ,9 ,15 ,16 ,17 ,22 ,23 ,21 ,24 ,25},
{10 ,15 ,16 ,17 ,19 ,18 ,22 ,23 ,21 ,24 ,25 ,28},
{7 ,10 ,9 ,8 ,15 ,18 ,19 ,16 ,17 ,22 ,23 ,21 ,24 ,25},
};

typedef enum {
TERM_PANIC, NONTERM_PANIC
} PanicType;

static void error(Token t);
static inline AstNode *make_node(int index);
static inline int action(int focus, Token lookahead);
static inline void handle_panic(PanicType type);

const Token empty = {};
const Token token_epsilon = {"epsilon", 8};

bool PANIC;
T word;
void *focus;
Stack *stack;

AstNode *parse(const char *source) {
    int i;

    // init lexer
    init_scanner(source);

    PANIC = FALSE;
    stack = Stack_init();
    Stack *ast_stack = Stack_init();
    NonterminalType subtype = {STMT_PROGRAM};
    AstNode *root = AstNode_init(AST_Stmt, subtype, empty, NONTERMINAL_NAME[0]);
    DArray *top = DArray_init(sizeof(AstNode), 2);
    DArray_push(top, 1);
    DArray_push(top, root);

    // init stack
    word = scan_token();
    Stack_push(stack, eof);
    Stack_push(stack, non_index);
    Stack_push(ast_stack, top);
    focus = Stack_top(stack);

    while (Stack_length(stack) > 0) {
        // log_info("cur focus: %s, cur word: %s", focus < non_index? token_type_string[(int)focus] : NONTERMINAL_NAME[(int)(focus - non_index)], token_type_string[word.type]);
        // TODO(ljr): finish parser
        if (focus == eof && word.type == T_EOF) {
            if (PANIC) return NULL;
            return root;
        } else if (focus < non_index || focus == T_EOF) {
            //
            // the top of the analysis stack is a terminal grammar
            //
            if (focus == word.type || focus == epsilon) {
                Stack_pop(stack);

                if (PANIC) {
                    if (focus != epsilon) word = scan_token();
                    focus = Stack_top(stack);
                    continue;
                }

                // at least proceed parent ast node
                // at most proceed multiple ancestors' nodes
                while (TRUE) {
                    // update and bookeep ast node information
                    DArray* nodes = Stack_top(ast_stack);
                    int cur_index = DArray_get(nodes, 0);
                    AstNode * node = DArray_get(nodes, cur_index);
                    check(node != NULL, "Failed to get node. nodes end is: %d", nodes->end);
                    // update attr.token if nonterminal
                    if (node->type == AST_Term) {
                        node->attr.token = focus == word.type ? word : token_epsilon;
                    }
                    // subtree handle finished, insert into parent node's ptr array
                    if (cur_index == nodes->end) {
                        // pop subtree
                        Stack_pop(ast_stack);
                        // might pop last element
                        if (Stack_length(ast_stack) == 0) {
                            break;
                        }
                        // get parent nodes
                        DArray* parent_nodes = Stack_top(ast_stack);
                        // get parent
                        int prev_index = DArray_get(parent_nodes, 0);
                        AstNode *parent = DArray_get(parent_nodes, prev_index);
                        // insert into ptr array
                        for (i = 1; i <= nodes->end; ++i) {
                            check(DArray_get(nodes, i), "Try to get node from null place");
                            DArray_push(parent->ptrs, DArray_get(nodes, i));
                        }
                        // release nodes
                        DArray_destroy(nodes);
                        //
                        // update parent_nodes' index in next iteration 
                        //
                    } else {
                        // index proceed
                        DArray_set(nodes, 0, cur_index+1);
                        break;
                    }
                }
                // lookahead token succeed
                if (focus != epsilon) word = scan_token();
            } else {
                handle_panic(TERM_PANIC);
            }

        } else {
            //
            // the top of the analysis stack is a non-terminal grammar
            //
            int next_production = action(focus, word);
            // log_info("next_production: %d", next_production);
            if (next_production > 0) {
                // init current level of ast subtree
                // first element set to the index of current processed node
                DArray *nodes = DArray_init(sizeof(AstNode), 10);
                DArray_push(nodes, (void *)1);
                check(nodes != NULL, "Failed to create nodes");
                // pop non-terminal
                Stack_pop(stack);
                // push each product
                for (i = 7; i >= 0; --i) {
                    // get each product
                    int product = productions[next_production][i];
                    // product exists and not epsilon
                    if (product > 0) {
                        // TODO(ljr): should we push epsilon here
                        // log_info("push %d %s", product, product < non_index? token_type_string[product] : NONTERMINAL_NAME[product - non_index]);
                        Stack_push(stack, (void *)product);
                        // push astnode into nodes
                        AstNode * n = make_node(product);
                        //
                        // for epsilon
                        //
                        if (product == epsilon) n->attr.token = token_epsilon;
                        // push into nodes
                        DArray_set(nodes, i + 1, n);
                    }
                }
                // log_info("push product finished.");
                // push nodes into ast_stack
                Stack_push(ast_stack, nodes);
            } else {
                // TODO(Ljr): failure recorvery
                handle_panic(NONTERM_PANIC);
            }
        }
        focus = Stack_top(stack);
    }

error:
    return NULL;
}

static inline int action(int focus, Token lookahead) {
    return LL_PARSE_TABLE[focus - non_index][lookahead.type];
}

// make a new AstNode based on the index and token
static inline AstNode *make_node(int index) {
    NodeType nodeType;
    NonterminalType subtype;
    char *name = NULL;
    if (index < non_index) {
        nodeType = AST_Term;
    } else if (index - non_index < EXPR_Bool) {
        nodeType = AST_Stmt; 
        subtype.stmt = index - non_index;
        name = NONTERMINAL_NAME[subtype.stmt];
    } else {
        nodeType = AST_Expr;
        subtype.expr = index - non_index;
        name = NONTERMINAL_NAME[subtype.expr];
    }
    return AstNode_init(nodeType, subtype, empty, name);
}

static inline void error(Token t) {
    fprintf(stderr, "error in (line: %d, pos: %d)\n", t.line_num, t.line_pos);
}

static inline void handle_panic(PanicType type) {
    Token cur;
    char tmp[100];
    int i, stack_top;

    cur = word;
    stack_top = (int)focus;
    PANIC = TRUE;

    if (type == TERM_PANIC) {
        // for terminal stack_top
        // if stack_top is T_EOF, report token missing and keep scanning
        // if stack_top is not T_EOF, report token and pop stack
        // log_warn("handle_panic(TERM_PANIC): %s", token_type_string[stack_top]);
        sprintf(tmp, "Missing token %s", token_type_string[stack_top]);
        report_error(SYNTAX_ERROR, word.line_num, word.line_pos, tmp);
        if (stack_top == T_EOF) word = scan_token();
        else Stack_pop(stack);
    } else {
        // for nonterminal stack_top, report unmatched and keep scanning token
        // until reach one in synchronized set of stack_top
        // then pop stack
        // log_warn("handle_panic(NONTERM_PANIC): %s", NONTERMINAL_NAME[stack_top - non_index]);
        int flag = 0;
        sprintf(tmp, "Failed matching %s%s", NONTERMINAL_NAME[stack_top - non_index], flag? "" : "");
        report_error(SYNTAX_ERROR, cur.line_num, cur.line_pos, tmp);

        while (word.type != T_EOF) {
            int index = stack_top - non_index;
            for (i = 0; SYNCHRONIZED_SET[index][i] != 0; ++i) {
                if (word.type == SYNCHRONIZED_SET[index][i]) {
                    // log_info("hit index: %d, num: %d", index, i);
                    flag = 1;
                    break;
                }
            }
            if (flag) {
                // current token may be valid to keep parsing
                break;
            } else {
                word = scan_token();
            }
        }
        Stack_pop(stack);
    }
}

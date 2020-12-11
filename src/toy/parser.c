#include <stdio.h>
#include "parser.h"
#include "scanner.h"
#include "ast.h"
#include "lib/stack.h"
#include "utils/bool.h"
#include "token.h"

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
const int productions[28][8] = {
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

static void error(Token t);
static inline AstNode *make_node(int index);
static inline int action(int focus, Token lookahead);

const Token empty = {};
const Token token_epsilon = {"epsilon", 8};

AstNode *parse(void) {
    int i;
    T word = scan_token();
    Stack *stack = Stack_init();
    Stack *ast_stack = Stack_init();
    NonterminalType subtype = {STMT_PROGRAM};
    AstNode *root = AstNode_init(AST_Stmt, subtype, empty, NONTERMINAL_NAME[0]);
    DArray *top = DArray_init(sizeof(AstNode), 2);
    DArray_push(top, 1);
    DArray_push(top, root);
    void *focus;
    // init stack
    Stack_push(stack, eof);
    Stack_push(stack, non_index);
    Stack_push(ast_stack, top);
    focus = Stack_top(stack);
    while (TRUE) {
        printf("cur focus: %s, cur word: %s\n", focus < non_index? token_type_string[(int)focus] : NONTERMINAL_NAME[(int)(focus - non_index)], token_type_string[word.type]);
        // TODO(ljr): finish parser
        if (focus == eof && word.type == T_EOF) {
            // success 
            return root;
        } else if (focus < non_index || focus == T_EOF) {
            if (focus == word.type || focus == epsilon) {
                // analysis stack
                Stack_pop(stack);
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
                        // printf("--------------\n");
                        // AstNode_print(node, 1);
                        // printf("--------------\n");
                    } else {
                        // index proceed
                        DArray_set(nodes, 0, cur_index+1);
                        break;
                    }
                }
                // lookahead token succeed
                if (focus != epsilon) word = scan_token();
                // printf("token: %.*s\n", word.length, word.start);
            } else {
                // TODO(ljr): failure recovery
                error(word);
                return NULL;
            }
        } else {
            int next_production = action(focus, word);
            printf("next_production: %d\n", next_production);
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
                        printf("push %d %s\n", product, product < non_index? token_type_string[product] : NONTERMINAL_NAME[product - non_index]);
                        Stack_push(stack, (void *)product);
                        // push astnode into nodes
                        AstNode * n = make_node(product);
                        //
                        // for epsilon
                        //
                        if (product == epsilon) n->attr.token = token_epsilon;
                        // push into nodes
                        DArray_set(nodes, i + 1, n);
                        // printf("set nodes: %d\n", i+1);
                    }
                }
                printf("push product finished.\n");
                // push nodes into ast_stack
                Stack_push(ast_stack, nodes);
            } else {
                // TODO(Ljr): failure recorvery
                error(word);
                return NULL;
            }
        }
        focus = Stack_top(stack);
    }
    sentinel("Can not reach here.");
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

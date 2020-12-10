#include "ast.h"
#include "stdlib.h"
#include "token.h"
#include "utils/debug.h"
#include "lib/darray.h"

#define T AstNode

T *AstNode_init(NodeType type, NonterminalType subtype, Token *token, char *s) {
    T *node;
    node = (T *)calloc(1, sizeof(T)); 
    check_mem(node);

    node->type = type;
    node->subtype = subtype;
    switch (type) {
    // non-terminal
    case AST_Nont:
        check(token != NULL, "Pass NULL token to AstNode init");
        node->attr.token = token;
        break;
    // expression
    case AST_Expr:
    // statement
    case AST_Stmt:
        check(s != NULL, "Pass NULL name str to AstNode init");
        node->attr.name = s;
        break;
    default:
        sentinel("Should not reach here.");
    }
    node->ptrs = (DArray *)DArray_init(sizeof(T *), 5);
    return node;
error:
    return NULL;
}

// record which children currently pointed to
void AstNode_record(T *node, int x) {
    node->index = x;
}

// clear both each node and ptrs in them
void AstNode_clear(T *root) {

}

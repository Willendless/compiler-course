#include "ast.h"
#include "stdlib.h"
#include "token.h"
#include "utils/debug.h"
#include "lib/darray.h"

#define T AstNode

T *AstNode_init(NodeType type, NonterminalType subtype, Token token, char *s) {
    T *node;
    node = (T *)calloc(1, sizeof(T)); 
    check_mem(node);

    node->type = type;
    node->subtype = subtype;
    switch (type) {
    // non-terminal
    case AST_Term:
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

// get kth child of node n
AstNode *AstNode_get_child(AstNode* n, int k) {
    check(n != NULL, "Node should not be null");
    check(n->ptrs->end >= k, "index beyond child num");
    return DArray_get(n->ptrs, k);
error:
    return NULL;
}

// naive implementation of printing ast tree
void AstNode_print(AstNode *root, int level) {
    int i;
    if (root == NULL) return;
    // print whitespace
    for (i = 0; i < level - 1; ++i) {
        printf("  ");
    }
    // printf terminal or non-terminal
    if (root->type == AST_Term) {
        printf("%.*s\n", root->attr.token.length, root->attr.token.start);
    } else {
        printf("%s\n", root->attr.name);
    }
    // call each child
    for (i = 0; i <= root->ptrs->end; ++i) {
        AstNode_print(DArray_get(root->ptrs, i), level + 1);
    }
}

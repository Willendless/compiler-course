#include "ast.h"
#include "stdlib.h"
#include "token.h"
#include "utils/debug.h"
#include "lib/darray.h"
#include "compiler.h"
#include "parser.h"

#define T AstNode

static T *gen_ast(T *);

static T *gen_for_stmt(T *);
static T *handle_if(T *);
static T *handle_while(T *);
static T *handle_assign(T *);
static T *handle_stmts(T *);

static T *gen_for_expr(T *);
static T *handle_arith_mult(T *);
static T *handle_prime(T *);
static T *handle_simple(T *);
static T *handle_boolexpr(T *);

static T *replicate_node(T *);

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
T *AstNode_get_child(T* n, int k) {
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

T *AstNode_gen_ast(T *root) {
    if (root == NULL) {
        log_warn("Pass NULL tree root");
        return NULL;
    }
    return gen_ast(root);
}

static T *gen_ast(T *root) {
    NonterminalType subtype;
    // handle terminal
    // 1. ignore epsilon leaf
    // 2. ignore {, }, ( and )
    if (root->type == AST_Term) {
        switch (root->attr.token.type) {
        case T_LEFT_BRACE:
        case T_RIGHT_BRACE:
        case T_LEFT_PAREN:
        case T_RIGHT_PAREN:
        case T_ERROR:
            return NULL;
        default:
            log_info("Gen terminal: %s", token_type_string[root->attr.token.type]);
            return replicate_node(root);
        }
    }
    if (root->type == AST_Stmt) {
        return gen_for_stmt(root);
    } else {
        return gen_for_expr(root);
    }
}

static T *gen_for_stmt(T *root) {
    switch (root->subtype.stmt) {
        case STMT_If: return handle_if(root);
        case STMT_While: return handle_while(root);
        case STMT_Assign: return handle_assign(root);
        case STMT_Stmts: return handle_stmts(root);
        case STMT_Compound:
            check(root->ptrs->end == 2,
                  "AstNode for STMT_Compound can only have 3 child.");
            return gen_ast(DArray_get(root->ptrs, 1));
        case STMT_PROGRAM:
        case STMT_Stmt:
            check(root->ptrs->end == 0,
                  "AstNode for Stmt and Program can only have one child.");
            return gen_ast(DArray_get(root->ptrs, 0));
        default: sentinel("Cannot reach here.");
    }
error:
    return NULL;
}

static T *replicate_node(T *n) {
    return AstNode_init(n->type, n->subtype, n->attr.token, n->attr.name);
}

// ifstmt/whilestmt -> if ( boolexpr ) then stmt else stmt
static T *handle_if(T *root) {
    check(root->ptrs->end == 7, "Failed to get if");
    T *n = replicate_node(root);
    T *_boolexpr = DArray_get(root->ptrs, 2);
    T *stmt1 = DArray_get(root->ptrs, 5);
    T *stmt2 = DArray_get(root->ptrs, 7);
    DArray_push(n->ptrs, gen_ast(_boolexpr));
    DArray_push(n->ptrs, gen_ast(stmt1));
    DArray_push(n->ptrs, gen_ast(stmt2));
    return n;
error:
    return NULL;
}

// whilestmt -> while ( boolexpr ) stmt
static T *handle_while(T *root) {
    check(root->ptrs->end == 4, "Failed to get while");
    T *n = replicate_node(root);
    T *_boolepxr = DArray_get(root->ptrs, 2);
    T *stmt = DArray_get(root->ptrs, 4);
    DArray_push(n->ptrs, gen_ast(_boolepxr));
    DArray_push(n->ptrs, gen_ast(stmt));
    return n;
error:
    return NULL;
}

// assgstmt -> ID = arithexpr;
static T *handle_assign(T *root) {
    log_info("Gen terminal: %s", NONTERMINAL_NAME[root->attr.token.type]);
    check(root->ptrs->end == 3, "Failed to get assign stmt, child num: %d instead of 3", root->ptrs->end + 1);
    T *n = replicate_node(root);
    T *id = replicate_node(DArray_get(root->ptrs, 0));
    T *expr = DArray_get(root->ptrs, 2);
    DArray_push(n->ptrs, id);
    DArray_push(n->ptrs, gen_ast(expr));
    return n;
error:
    return NULL;
}

// stmts -> stmt stmts | epsilon
static T *handle_stmts(T *root) {
    check(root->ptrs->end == 1, "Failed to get stmts");
    T *n = replicate_node(root);
    while (root->ptrs->end == 1) {
        T *child = gen_ast(DArray_get(root->ptrs, 0));
        if (child) {
            DArray_push(n->ptrs, child);
        }
        root = DArray_get(root->ptrs, 1);
    }
    // all stmts are epsilon
    // free this new node
    if (n->ptrs->end == -1) {
        free(n);
        log_info("Free node since all stmts are epsilon");
        n = NULL;
    }
    return n;
error:
    return NULL;
}

static T *gen_for_expr(T *root) {
    switch (root->subtype.expr) {
    case EXPR_Arith:
    case EXPR_Mult: return handle_arith_mult(root);
    case EXPR_ArithPrime: 
    case EXPR_MultPrime: return handle_prime(root);
    case EXPR_Simple: return handle_simple(root);
    case EXPR_Bool: return handle_boolexpr(root);
    case EXPR_BoolOp:
        check(root->ptrs->end == 0,
        "AstNode for Bool expr can only have one child.");
        return gen_ast(DArray_get(root->ptrs, 0));
    default:
        sentinel("should not reach here.");
        break;
    }
error:
    return NULL;
}

// arithexpr -> multexpr arithexprprime
// multexpr -> simpleexpr multexprprime
// 生成左子树、右子树，右子树若非空，则以右子树根为根
static T *handle_arith_mult(T *root) {
    T *ret;
    T *left;
    check(root->ptrs->end == 1, "Child num of root should be 2.");
    left = gen_ast(DArray_get(root->ptrs, 0));
    ret = gen_ast(DArray_get(root->ptrs, 1));
    if (ret) {
        DArray_set(ret->ptrs, 0, left);
    } else {
        ret = left;
    }
    return ret;
error:
    return NULL;
}

// arithexprprime -> + multexpr arithexprprime | - multexpr arithexprprime | epsilon
// multexprprime -> * simpleexpr multexprprime | / simpleexpr multexprprime | epsilon
// 以第一个子节点为根，
// 1. 若第3个子节点非epsilon, 则以第三个子节点的第一个儿子为右子树的根，返回值作为右节点
// multexpr作为左节点
// 2. 若第三个子节点为epsilon, 则以multexpr作为右节点
// 返回一个左子树缺少，右子树完整的根
static T *handle_prime(T *root) {
    T *ret;
    T *second, *third;
    // epsilon root
    if (root->ptrs->end == 0) return NULL;
    check(root->ptrs->end == 2, "Child num of root should be 3.");
    // construct new root
    ret = replicate_node(DArray_get(root->ptrs, 0));
    // gen second and third
    second = gen_ast(DArray_get(root->ptrs, 1));
    third = gen_ast(DArray_get(root->ptrs, 2));
    // if the third child is not epsilon, then
    // the root of the right subtree should be the operator of the third child
    // other wise the right subtree should be the return value of the second child
    if (third != NULL) {
        // not epsilon
        T *right = replicate_node(DArray_get(((T *)DArray_get(root->ptrs, 2))->ptrs, 0));
        DArray_push(right->ptrs, second);
        DArray_push(right->ptrs, third);
        DArray_set(ret->ptrs, 1, right);
    } else {
        // epsilon
        DArray_set(ret->ptrs, 1, second);
    }
    return ret;
error:
    return NULL;
}

// boolexpr -> arithexpr boolop arithexpr
// 以boolop为根，左右为子树
static T *handle_boolexpr(T *root) {
    T *ret;
    T *left, *right;
    check(root->ptrs->end == 2, "Child num of root should be 3.");
    ret = replicate_node(gen_ast(DArray_get(root->ptrs, 1)));
    // log_info("Gen bool: %s", );
    left = gen_ast(DArray_get(root->ptrs, 0));
    right = gen_ast(DArray_get(root->ptrs, 2));
    DArray_push(ret->ptrs, left);
    DArray_push(ret->ptrs, right);
    return ret;
error:
    return NULL;
}

// simpleexpr -> ID | NUM | ( arithexpr )
// 若为叶子，直接返回
// 否则返回arithexpr
static T *handle_simple(T *root) {
    if (root->ptrs->end == 0) {
        return gen_ast(DArray_get(root->ptrs, 0));
    } else if (root->ptrs->end == 2) {
        return gen_ast(DArray_get(root->ptrs, 1));
    } else {
        sentinel("Should not reach here.");
    }
error:
    return NULL;
}

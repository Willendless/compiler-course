#include "semantic_analysis.h"
#include "symtable.h"
#include "utils/bool.h"
#include "utils/debug.h"
#include "compiler.h"

static void handle_stmt(AstNode *);
static void handle_expr(AstNode *);
static void analyze(AstNode *);

static bool ERROR;

bool semantic_analysis(AstNode *root) {
    if (root == NULL) return FALSE;

    ERROR = FALSE;
    Symtable_init(500);
    analyze(root);
    Symtable_destroy();
    return ERROR;
}

static void analyze(AstNode *root) {
    int i;
    // traverse syntax tree
    switch (root->type) {
    case AST_Stmt: handle_stmt(root); break;
    case AST_Expr: handle_expr(root); break;
    default:
        // terminal
        return;
    }
    // keep traversing
    for (i = 0; i <= root->ptrs->end; ++i) {
        AstNode *n = DArray_get(root->ptrs, i);
        if (n->type != AST_Term) analyze(n);
    }
}

static void handle_stmt(AstNode *n) {
    AstNode *child;
    switch (n->subtype.stmt) {
    case STMT_Assign:
        child = AstNode_get_child(n, 0);
        check(child, "Get NULL child");
        check(child->type == AST_Term
                && child->attr.token.type == T_IDENTIFIER,
                "Unmatched child node T_IDENTIFIER");
        Symtable_insert(child->attr.token.start);
        break;
    default:
        break;
    }
error:
    return;
}

static void handle_expr(AstNode *n) {
    AstNode *child;
    Token t;
    switch (n->subtype.expr) {
    case EXPR_Simple:
        child = AstNode_get_child(n, 0);
        t = child->attr.token;
        check(child, "Get NULL child");
        if (child->type == AST_Term
            && t.type == T_IDENTIFIER
            && !Symtable_search(t.start)) {
                char tmp[100];
                sprintf(tmp, "Undefined identifier \"%.*s\"", t.length, t.start);
                report_error(SEMANTIC_ERROR, t.line_num, t.line_pos, tmp);
                ERROR = TRUE;
        }
    default:
        break;
    }
error:
    return;
}

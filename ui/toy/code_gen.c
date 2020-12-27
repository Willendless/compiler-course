#include "code_gen.h"
#include "utils/bool.h"
#include "utils/debug.h"
#include <stdarg.h>
#include "utils/utils.h"
#include "symtable.h"

#define T AstNode

int tempvar_index;
int label_index;
int var_index;

typedef int TempVar;
typedef int Label;

Operand empty;

static char* IR_OP_STR[] = {
    "+", "-", "*", "/", "=",
    "==", "<", "<=", ">", ">=",
    "goto", "label"
};

// translate stmt
static InterCode *translate_stmt(T *);
static InterCode *translate_stmt_if(T *);
static InterCode *translate_stmt_while(T *);
static InterCode *translate_stmt_assign(T *);
static InterCode *translate_stmt_stmts(T *);

// translate expr
static InterCode *translate_expr_arith(T *, TempVar);
static InterCode *translate_expr_cond(T *, Label, Label);

// new label
static Label new_label(void);
// new temp var
static TempVar new_tempvar(void);

// helper function
static bool is_expr(T *);
static OperandKind get_operandkind(T *node); 
static InterCodeOp get_opkind(T *node); 
static Operand *make_operand(OperandKind kind, void *attr);
static Operand *make_operand_by_node(T *);
static Operand *make_operand_by_label(Label);
static Operand *make_operand_by_temp(TempVar);
static InterCode *make_ir(InterCodeOp op, Operand *arg1, Operand *arg2, Operand *result);
static InterCode *combine_code(InterCode *c1, InterCode *c2);
static InterCode *combine_codes(int, ...);
// print ir func
static void print_ir(FILE *f, InterCode *);
static void print_ir_kind(FILE *f, InterCode *);
static void print_ir_goto(FILE *f, InterCode *);
static void print_ir_label(FILE *f ,InterCode *);
static void print_ir_assign(FILE *f, InterCode *);
static void print_ir_binop(FILE *f, InterCode *);
static void print_ir_cond(FILE *f, InterCode *);
static inline void get_name_from_operand(char *name, Operand *operand);

DArray *code;

DArray *code_generation(AstNode *root, FILE *f) {
    InterCode *ir;
    check(root != NULL, "Fail to generate code for null ast tree.");
    code = DArray_init(sizeof(InterCode), 10);
    tempvar_index = 1;
    label_index = 1;
    var_index = 1;
    ir = translate_stmt(root);
    print_ir(f, ir);
    return code;
error:
    return NULL;
}

static InterCode *translate_stmt(T *root) {
    check(root->type == AST_Stmt, "Pass non stmt node to stmt translate routine");
    switch (root->subtype.stmt) {
    case STMT_Assign: return translate_stmt_assign(root);
    case STMT_If: return translate_stmt_if(root);
    case STMT_While: return translate_stmt_while(root);
    case STMT_Stmts: return translate_stmt_stmts(root);
    default:
        sentinel("Should not reach here.");
        break;
    }
error:
    return NULL;
}

static InterCode *translate_stmt_assign(T *root) {
    T *head = DArray_get(root->ptrs, 0);
    T *body = DArray_get(root->ptrs, 1);
    Operand *result = make_operand_by_node(head);
    InterCode *code1, *code2, *code;
    if (is_expr(body)) {
        TempVar t1 = new_tempvar();
        code1 = translate_expr_arith(body, t1);
        code2 = make_ir(OP_ASSIGN, make_operand(OPERAND_TEMP, t1),
                        NULL, result);
        code = combine_code(code1, code2);
    } else {
        Operand *arg1 = make_operand_by_node(body);
        code = make_ir(OP_ASSIGN, arg1, NULL, result);
    } 
    return code;
}

// code: if [] goto l_true
//       goto l_false
//       LABLE l_true------------------1
//       [stmt]
//       goto l_end--------------------2
//       LABLE l_false-----------------3
//       [stmt]
//       LABLE l_end-------------------4
static InterCode *translate_stmt_if(T *root) {
    Label l1 = new_label();
    Label l2 = new_label();
    Label l3 = new_label();
    T *cond = DArray_get(root->ptrs, 0);
    T *body1 = DArray_get(root->ptrs, 1);
    T *body2 = DArray_get(root->ptrs, 2);
    InterCode *c1 = translate_expr_cond(cond, l1, l2);
    InterCode *c2 = translate_stmt(body1);
    InterCode *c3 = translate_stmt(body2);
    InterCode *a = make_ir(OP_LABEL, NULL, NULL, make_operand_by_label(l1));
    InterCode *b = make_ir(OP_GOTO, NULL, NULL, make_operand_by_label(l3));
    InterCode *c = make_ir(OP_LABEL, NULL, NULL, make_operand_by_label(l2));
    InterCode *d = make_ir(OP_LABEL, NULL, NULL, make_operand_by_label(l3));
    return combine_codes(7, c1, a, c2, b, c ,c3, d);
}

// l_loop----------------1
// if [] goto l_true
// goto l_false
// LABEL l_true----------2
// [stmt]
// goto l_loop-----------3
// LABEL l_false---------4
static InterCode *translate_stmt_while(T *root) {
    Label l1 = new_label();
    Label l2 = new_label();
    Label l3 = new_label();
    T *cond = DArray_get(root->ptrs, 0);
    T *body = DArray_get(root->ptrs, 1);
    InterCode *c1 = translate_expr_cond(cond, l2, l3);
    InterCode *c2 = translate_stmt(body);
    InterCode *a = make_ir(OP_LABEL, NULL, NULL, make_operand_by_label(l1));
    InterCode *b = make_ir(OP_LABEL, NULL, NULL, make_operand_by_label(l2));
    InterCode *c = make_ir(OP_GOTO, NULL, NULL, make_operand_by_label(l1));
    InterCode *d = make_ir(OP_LABEL, NULL, NULL, make_operand_by_label(l3));
    return combine_codes(6, a, c1, b, c2, c, d);
}

static InterCode *translate_stmt_stmts(T *root) {
    int i;
    InterCode *code = translate_stmt(DArray_get(root->ptrs, 0));
    for (i = 1; i <= root->ptrs->end; ++i) {
        code = combine_code(code, translate_stmt(DArray_get(root->ptrs, i)));
    }
    return code;
}

static InterCode *translate_expr_arith(T *root, TempVar t) {
    InterCodeOp opkind = get_opkind(root);
    T *left = DArray_get(root->ptrs, 0);
    T *right = DArray_get(root->ptrs, 1);
    InterCode *c1, *c2, *c3;
    c1 = c2 = c3 = NULL;
    c3 = make_ir(opkind, NULL, NULL, make_operand_by_temp(t));
    if (is_expr(left)) {
        TempVar t1 = new_tempvar();
        c1 = translate_expr_arith(left, t1);
        c3->arg1 = make_operand_by_temp(t1);
    } else {
        c3->arg1 = make_operand_by_node(left);
    }
    if (is_expr(right)) {
        TempVar t2 = new_tempvar();
        c2 = translate_expr_arith(right, t2);
        c3->arg2 = make_operand_by_temp(t2);
    } else {
        c3->arg2 = make_operand_by_node(right);
    }
    return combine_codes(3, c1, c2, c3);
}

static InterCode *translate_expr_cond(T *root, Label l_true, Label l_false) {
    InterCodeOp opkind = get_opkind(root);
    T *left = DArray_get(root->ptrs, 0);
    T *right = DArray_get(root->ptrs, 1);
    InterCode *c1, *c2, *c3, *c4;
    c1 = c2 = c3 = c4 = NULL;
    c3 = make_ir(opkind, NULL, NULL, make_operand_by_label(l_true));
    c4 = make_ir(OP_GOTO, NULL, NULL, make_operand_by_label(l_false));
    if (is_expr(left)) {
        TempVar t = new_tempvar();
        c1 = translate_expr_arith(left, t);
        c3->arg1 = t;
    } else {
        c3->arg1 = make_operand_by_node(left);
    }
    if (is_expr(right)) {
        TempVar t = new_tempvar();
        c2 = translate_expr_arith(right, t);
        c3->arg2 = t;
    } else {
        c3->arg2 = make_operand_by_node(right);
    }
    return combine_codes(4, c1, c2, c3, c4);
}


static bool is_expr(T *node) {
    check(node != NULL, "Null pointer.");
    return node->attr.token.type != T_IDENTIFIER
            && node->attr.token.type != T_NUMBER;
error:
    return FALSE;
}

static Label new_label(void) {
    return (Label)label_index++;
}

static TempVar new_tempvar(void) {
    return (TempVar)tempvar_index++;
}

static InterCode *make_ir(InterCodeOp op, Operand *arg1, Operand *arg2, Operand *result) {
    InterCode *i = (InterCode *)calloc(1, sizeof(InterCode));
    check_mem(i);
    i->op = op;
    i->arg1 = arg1;
    i->arg2 = arg2;
    i->result = result;
    i->next = i;
    i->prev = i;
    return i;
error:
    return NULL;
}

static Operand *make_operand_by_node(T *node) {
    return make_operand(get_operandkind(node), &node->attr.token);
}

static Operand *make_operand_by_temp(TempVar t) {
    return make_operand(OPERAND_TEMP, t);
}

static Operand *make_operand_by_label(Label t) {
    return make_operand(OPERAND_LABEL, t);
}

static Operand *make_operand(OperandKind kind, void *attr) {
    Operand *operand = (Operand *)calloc(1, sizeof(Operand));
    operand->kind = kind;
    if (kind == OPERAND_TEMP) {
        operand->attr.tempvar_index = (int)attr;
    } else if (kind == OPERAND_LABEL) {
        operand->attr.label_index = (int)attr;
    } else if (kind == OPERAND_CONST) {
        operand->attr.var_index.token = *(Token *)attr;
    } else {    
        char s[100];
        Token t = *(Token *)attr;
        sprintf(s, "%.*s", t.length, t.start);
        operand->attr.var_index.index = Symtable_lookup(s)->n;
        operand->attr.var_index.token = t;
    }
    return operand;
}

static InterCodeOp get_opkind(T *node) {
    Token t;
    check(node->type == AST_Term, "Op node type should be AST_Term");
    t = node->attr.token;
    switch (t.type) {
    case T_PLUS: return OP_ADD;
    case T_MINUS: return OP_SUB;
    case T_STAR: return OP_MUL;
    case T_SLASH: return OP_DIV;
    case T_GREATER: return OP_GREATE;
    case T_GREATER_EQUAL: return OP_GREATE_E;
    case T_LESS: return OP_LESS;
    case T_LESS_EQUAL: return OP_LESS_E;
    case T_EQUAL_EQUAL: return OP_EQUAL;
    default:
        sentinel("Should not reach here.");
        break;
    }
error:
    return OP_ERROR;
}

static OperandKind get_operandkind(T *node) {
    Token t;
    check(node->type == AST_Term, "Operand node type should be AST_Term");
    t = node->attr.token;
    if (t.type == T_IDENTIFIER) return OPERAND_VAR;
    else if (t.type == T_NUMBER) return OPERAND_CONST; 
    else sentinel("should not reach here");
error:
    return OPERAND_TEMP;
}

// combine two linked list
static InterCode *combine_code(InterCode *c1, InterCode *c2) {
    if (c1 == NULL) return c2;
    if (c2 == NULL) return c1;
    // record old tail
    InterCode *t = c1->prev;
    // connect new head and tail
    c1->prev = c2->prev;
    c2->prev->next = c1;
    // handle old tail and head
    t->next = c2;
    c2->prev = t;
    return c1;
}

static InterCode *combine_codes(int cnt, ...) {
    int i;
    bool flag = FALSE;
    InterCode *head, *p;
    va_list ap;
    va_start(ap, cnt);
    p = va_arg(ap, InterCode *);
    for (i = 0; i < cnt; ++i, p = va_arg(ap ,InterCode *)) {
        if (p && !flag) {
            head = p;
            flag = TRUE;
        } else if (p) {
            head = combine_code(head, p);
        }
    }
    return head;
}

static void print_ir(FILE *f, InterCode *c) {
    InterCode *p = c;

    do {
        print_ir_kind(f, p);
        // push each ir into code
        DArray_push(code, p);
        p = p->next;
    } while (p != c);
}

static void print_ir_kind(FILE *f, InterCode *ir) {
    switch (ir->op) {
    case OP_GOTO: print_ir_goto(f, ir); break;
    case OP_LABEL: print_ir_label(f, ir); break;
    case OP_ASSIGN: print_ir_assign(f, ir); break;
    default: 
        if (ir->op <= OP_DIV) print_ir_binop(f, ir);
        else print_ir_cond(f, ir);
        break;
    }
}

static void print_ir_goto(FILE *f, InterCode *ir) {
    fprintf(f, "goto LABEL%d\n", ir->result->attr.label_index);
}

static void print_ir_label(FILE *f, InterCode *ir) {
    fprintf(f, "LABEL%d:\n", ir->result->attr.label_index);
}

static void print_ir_assign(FILE *f, InterCode *ir) {
    char head[50], arg1[50];
    get_name_from_operand(head, ir->result);
    get_name_from_operand(arg1, ir->arg1);
    fprintf(f, "%s = %s\n", head, arg1);
}

static void print_ir_binop(FILE *f, InterCode *ir) {
    char head[50], arg1[50], arg2[50];
    get_name_from_operand(head, ir->result);
    get_name_from_operand(arg1, ir->arg1);
    get_name_from_operand(arg2, ir->arg2);
    fprintf(f, "%s = %s %s %s\n", head, arg1, IR_OP_STR[ir->op], arg2);
}

static void print_ir_cond(FILE *f, InterCode *ir) {
    char arg1[50], arg2[50];
    get_name_from_operand(arg1, ir->arg1);
    get_name_from_operand(arg2, ir->arg2);
    fprintf(f, "if %s %s %s goto LABEL%d\n", arg1, IR_OP_STR[ir->op], arg2, ir->result->attr.label_index);
}

static inline void get_name_from_operand(char *name, Operand *operand) {
    if (operand->kind == OPERAND_TEMP) {
        sprintf(name, "t%d", operand->attr.tempvar_index);
    } else if (operand->kind == OPERAND_VAR || operand->kind == OPERAND_CONST) {
        Token t = operand->attr.var_index.token;
        sprintf(name, "%.*s", t.length, t.start);
    }
}

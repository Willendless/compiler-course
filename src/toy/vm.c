#include "vm.h"
#include "code_gen.h"
#include "lib/table.h"
#include "lib/darray.h"
#include "utils/utils.h"
#include "utils/debug.h"
#include "symtable.h"

typedef struct {
    int pc;
    DArray *code;
    DArray *label;
    DArray *var;
    DArray *temp_var;
    DArray *var_name;
} Vm;

Vm vm;

static void vm_init(DArray *code) {
    vm.pc = 0;
    vm.code = code;
    vm.label = DArray_init(sizeof(int), 50);
    vm.temp_var = DArray_init(sizeof(double), 100);
    vm.var = DArray_init(sizeof(double), Symtable_length() + 1);
    vm.var_name = DArray_init(sizeof(Token), Symtable_length()+1);
    // log_info("Symtable length: %d", Symtable_length());
    // dummy node
    DArray_push(vm.label, 1);
    DArray_push(vm.temp_var, (long)1.0);
    DArray_push(vm.var, (long)1.0);
}

// rec position of every label
static void vm_preprocess() {
    int i;
    for (i = 0; i <= vm.code->end; ++i) {
        InterCode *c = DArray_get(vm.code, i);
        switch (c->op) {
        case OP_LABEL: DArray_push(vm.label, i);
        case OP_ASSIGN:
            if (c->result->kind == OPERAND_VAR)
                DArray_set(vm.var_name, c->result->attr.var_index.index, &(c->result->attr.var_index.token));
        default: break;
        }
   }
}

static void vm_print() {
    int i;
    char s[100];
    for (i = 1; i <= vm.var_name->end; ++i) {
        Token k = *(Token *)DArray_get(vm.var_name, i);
        sprintf(s, "%.*s", k.length, k.start);
        printf("%s: %d\n", s, DArray_get(vm.var, Symtable_lookup(s)->n));
    }
}

static void vm_clear() {
    DArray_destroy(vm.label);
    DArray_destroy(vm.var);
    DArray_destroy(vm.temp_var);
}

static void * get_operand_value(Operand *operand) {
    char val[100];
    Token t;
    switch (operand->kind) {
    case OPERAND_TEMP:
        return DArray_get(vm.temp_var, operand->attr.tempvar_index);
    case OPERAND_VAR:
        return DArray_get(vm.var, operand->attr.var_index.index);
    case OPERAND_CONST:
        t = operand->attr.var_index.token;
        sprintf(val, "%.*s", t.length, t.start);
        return atol(val);
    case OPERAND_LABEL:
        return DArray_get(vm.label, operand->attr.label_index);
    default:
        sentinel("Should not reach here");
    }
error:
    return NULL;
}

static void set_operand(Operand *res, void *val) {
    switch (res->kind) {
    case OPERAND_TEMP:
        DArray_set(vm.temp_var, res->attr.tempvar_index, val);
        // printf("set temp var: %d\n", res->attr.tempvar_index);
        return;
    case OPERAND_VAR:
        // printf("set var: %d\n", res->attr.var_index.index);
        DArray_set(vm.var, res->attr.var_index.index, val);
        return;
    default:
        sentinel("Should not reach here.");
    }
error:
    return;
}

static void exe_goto(InterCode *c) {
    check(c->result->kind == OPERAND_LABEL, "argument should be OPPERAND_LABEL");
    vm.pc = (int)get_operand_value(c->result);
error:
    return;
}

void vm_run(DArray *code) {
    int i;

    if (code == NULL) {
        return;
    }
    vm_init(code);
    vm_preprocess();

    // instruction dispatch
    for (; vm.pc <= vm.code->end;) {
        InterCode *c = DArray_get(code, vm.pc);
        if (c->op <= OP_ASSIGN) {
            // biexpr
            long res = 0;
            switch (c->op) {
            case OP_ADD: res = (long)get_operand_value(c->arg1) + (long)get_operand_value(c->arg2); break;
            case OP_SUB: res = (long)get_operand_value(c->arg1) - (long)get_operand_value(c->arg2); break;
            case OP_MUL: res = (long)get_operand_value(c->arg1) * (long)get_operand_value(c->arg2); break;
            case OP_DIV: res = (long)get_operand_value(c->arg1) / (long)get_operand_value(c->arg2); break;
            default: // OP_ASSIGN
                res = get_operand_value(c->arg1);
            }
            set_operand(c->result, res);
            vm.pc++;
        } else if (c->op == OP_GOTO) {
            exe_goto(c);
        } else {
            bool goto_label = FALSE;
            switch (c->op) {
            case OP_GREATE: goto_label = (long)get_operand_value(c->arg1) > (long)get_operand_value(c->arg2); break;
            case OP_GREATE_E: goto_label = (long)get_operand_value(c->arg1) >= (long)get_operand_value(c->arg2); break;
            case OP_LESS: goto_label = (long)get_operand_value(c->arg1) < (long)get_operand_value(c->arg2); break;
            case OP_LESS_E: goto_label = (long)get_operand_value(c->arg1) <= (long)get_operand_value(c->arg2); break;
            default: break; // OP_LABEL
            }
            if (goto_label) exe_goto(c); else vm.pc++;
        } 
    }
    vm_print();
    vm_clear();
}
